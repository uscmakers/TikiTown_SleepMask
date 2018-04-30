/*
 * NOTE: This program should work while connected via USB
 * it is suggested that debugging be done over USB and when 
 * connected over Bluetoooth to comment out all serial.print 
 * statements except for specific instructions to the devices
 * that it is connected to
 * 
 * The recommended python program to interface with the arduino 
 * over usb is serialcomm.py. It is important to have python3
 * on the computer and use the following command to run the program
 * 
 * sudo python3 serialcomm.py on mac 
 * or python3 serialcommpy on pc *
 * 
 * *for pc the terminal must be open on administrator mode
 * 
 * It is important to note that all incoming data send to the 
 * adruino must be sent in the form of binary values rather than
 * ascii values and in the right format.
 */

#include <MPU6050.h>

#include <Wire.h>
#include <ControlRGB.h>

#define MAX_BUFF_SIZE 8
#define START_PACKET_SIZE 8
#define QUICK_START_PACKET_SIZE 1
#define QUICK_STOP_PACKET_SIZE 1
#define STOP_PACKET_SIZE 6
#define QUIT_PACKET_SIZE 1  
#define ledPin 3
#define onBoardLed 13
#define button 10
//#define THRES 2

typedef struct __attribute__((packed)){ //8 bytes
    unsigned int n_s; //num of samples
    unsigned int s_d; //sampling delay
    unsigned int p_d; //pulse delay
    unsigned char threshhold;
} start_pkt;

typedef struct __attribute__((packed)){ //6 bytes
    unsigned char rBrightness;
    unsigned char gBrightness;
    unsigned char bBrightness;
    unsigned int ramptime;
} stop_pkt;

char serialInput[MAX_BUFF_SIZE];
volatile boolean stringComplete = false;  // whether the string is complete
volatile boolean stringStarted = false;
volatile unsigned char cmd = 0;
volatile boolean routineStarted = false;
boolean readdata = false;
volatile int i = 0;
volatile int pktSize = 0;
volatile start_pkt startData;
volatile stop_pkt stopData;
MPU6050 accel;
Vector accelerationVector;
double accelValue, prevValue, deltaValue;;
int cnt = 0;
int btnval = 0;
ControlRGB ledControl = ControlRGB(3,5,6);

void setup() {
  // initialize serial:
//pinMode(7, OUTPUT);
  pinMode(onBoardLed, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  Serial.begin(9600);
  while(!accel.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
//    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
}

void loop() {
  btnval = digitalRead(button);
  // print the string when a newline arrives:
  if (btnval == LOW && cmd == 2){
    cmd = 3;
    stringComplete = true;
  }
  if (stringComplete || routineStarted) { 
    //routineStarted is only set to 1 when cmd 1 or 4 is sent to allow the code to loop through the
    //cmd == 1 || cmd == 4 case
    
    if(cmd == 1 || cmd == 4){
        /*************************light sleep detection algorithm******************/
            /*When connected via bluetooth, any serial.print statements
            //will be sent over bluetooth. Currently, the arduino is set
            //to send a binary 1 when the algorithm detects that the user
            //is in light sleep. The devices would then recieve the 1 and 
            //determine if it's time to wake the user up by sending a 
            //a binary 2 along with wake up parameters, or it would send 
            //a binary 5 to wake up using the default wake up parameters*/ 
                       
        cnt = 0;
        digitalWrite(onBoardLed, HIGH);
        
        for (unsigned int j = 0; j < startData.n_s; j++) {
          accelerationVector = accel.readNormalizeGyro();
          prevValue = accelValue;
          accelValue = pow(pow(accelerationVector.XAxis,2) + pow(accelerationVector.YAxis,2) + pow(accelerationVector.ZAxis,2),0.5);
          deltaValue = abs(accelValue - prevValue);
         
          if (deltaValue > startData.threshhold) {
            cnt += 1;
          }
          
          if (cnt > (0.3 * startData.n_s)) {  
            Serial.write(1); //SENDS TRIGGER
            break;
          }
          delay(startData.s_d);
        }
        digitalWrite(onBoardLed, LOW);
        delay(startData.p_d);
        /****************************************************************************/
    }
    else if(cmd == 2 || cmd == 5){
        /*****************************User wake up stage****************************/
            //TODO: Interface with Julia's led library for waking the user up
            //using the multicolor LEDs
        ledControl.turnOn(stopData.rBrightness, stopData.gBrightness, stopData.bBrightness, stopData.ramptime);
        /****************************************************************************/
    }
    else if(cmd == 3){
        /*****************************Turn off LEDs*********************************/
        ledControl.turnOff();
        cmd = 0;
        
        /****************************************************************************/
    }
    // clear the data:
    //
    stringComplete = false;
    stringStarted = false;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = Serial.read();
    serialInput[i]= inChar;
    i++;
    routineStarted = false;

    //Sees if this is the start of a string, if so the first byte is 
    //set as the cmd and the pktSize is set so the program knows how 
    //many bytes to recieve before it is done
    if (stringStarted == false){
      cmd = inChar;
      stringStarted = true;
      switch (cmd){
        case(1):
          pktSize = START_PACKET_SIZE;
          break;
        case(2):
          pktSize = STOP_PACKET_SIZE;
          break;
        case(3):
          pktSize = QUIT_PACKET_SIZE;
          break;
        case(4):
          pktSize = QUICK_START_PACKET_SIZE;
          break;
        case(5):
          pktSize = QUICK_STOP_PACKET_SIZE;
          break;
        default:
          stringStarted = false;
          break;
      }
    }

    //Determine parameters stage: any data following the initial cmd byte will
    //be converted and stored in the appropriate template
    if (i >= pktSize) {
      stringComplete = true;
      switch (cmd){
        case(1): 
          startData = *((start_pkt *)(serialInput + 1));
          routineStarted = true;
          break;
        case(2):
          stopData = *((stop_pkt *)(serialInput + 1));
          break;
        case(4):
          startData = {10,100,1000, 2};
          routineStarted = true;
          break;
        case(5):
          stopData = {255, 128, 0, 7500};
          break;
        default:
          break;
      }
      memset(serialInput, 0, MAX_BUFF_SIZE);
      i=0;
    }
  }
}

void checkAccelSettings()
{
  Serial.println();
  
  Serial.print(" * Sleep Mode:            ");
  Serial.println(accel.getSleepEnabled() ? "Enabled" : "Disabled");
  
  Serial.print(" * Clock Source:          ");
  switch(accel.getClockSource())
  {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }
  
  Serial.print(" * Accelerometer:         ");
  switch(accel.getRange())
  {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }  

  Serial.print(" * Accelerometer offsets: ");
  Serial.print(accel.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(accel.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(accel.getAccelOffsetZ());
  
  Serial.println();
}

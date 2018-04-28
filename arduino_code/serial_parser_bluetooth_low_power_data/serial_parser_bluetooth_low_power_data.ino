#include <MPU6050.h>

#include <Wire.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define MAX_BUFF_SIZE 8
#define START_PACKET_SIZE 3
#define STOP_PACKET_SIZE 1
#define QUIT_PACKET_SIZE 1  
#define ledPin 3
#define onBoardLed 13
#define button 10
#define SLEEP_EN 0
#define THRES_EN 1
#define THRES 6
//#define THRES 0.7
#define TEST_MODE 0

typedef struct __attribute__((packed)){
    unsigned int sample_delay; 
} start_pkt;

typedef struct __attribute__((packed)){
    unsigned char brightness;
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
double accelValue = 0;
double prevValue = 0;
double deltaValue;
int cnt = 0;
int holdcnt = 0;
int btnval = 0;
int idlecount = 0;
boolean trig = false;

void wakeUpNow()        // here the interrupt is handled after wakeup
{
	idlecount = 0;
	digitalWrite(onBoardLed, HIGH);
  trig = true;
}


void sleepNow()         // here we put the arduino to sleep
{
    //accel.setSleepEnabled(true);
	  digitalWrite(onBoardLed, LOW);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin
 
    //attachInterrupt(digitalPinToInterrupt(2),wakeUpNow, RISING); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW
 
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
 
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    //detachInterrupt(digitalPinToInterrupt(2));      // disables interrupt 0 on pin 2 so the
                             // wakeUpNow code will not be executed
                             // during normal running time.
}

void setup() {
  	// initialize serial:
	// pinMode(7, OUTPUT);
  if(SLEEP_EN){
    //attachInterrupt(digitalPinToInterrupt(2),wakeUpNow, RISING);
  }
	pinMode(onBoardLed, OUTPUT);
	pinMode(button, INPUT_PULLUP);

	Serial.begin(9600);
 
  while(!accel.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
  
  accel.setAccelPowerOnDelay(MPU6050_DELAY_3MS);

  accel.setIntFreeFallEnabled(false);  
  accel.setIntZeroMotionEnabled(false);
  accel.setIntMotionEnabled(SLEEP_EN == 1);

  if(!SLEEP_EN){
    accel.setDLPFMode(MPU6050_DLPF_6);
  }
  
  accel.setMotionDetectionThreshold(1);
  accel.setMotionDetectionDuration(1);

  //accel.setSleepEnabled(1);
  
  //Serial.println("XYZ Offsets:");
  //Serial.println(accel.getAccelOffsetX());
  //Serial.println(accel.getAccelOffsetY());
  //Serial.println(accel.getAccelOffsetZ());

  if(TEST_MODE){
    cmd = 1;
    routineStarted = true;
  }
	// checkAccelSettings();
}

void loop() {
    if(SLEEP_EN)
      digitalWrite(onBoardLed, HIGH);
    
    if (digitalRead(button) == LOW && cmd == 0){
      delay(10);
      while(digitalRead(button) == LOW){delay(100);holdcnt++;}
      delay(10);
      if(holdcnt > 2){
        cmd = 1;
        startData = {100};
        stringComplete = true;
        routineStarted = true;
      }
      holdcnt = 0;
    }
  
    if (digitalRead(button) == LOW && cmd == 1){
      delay(10);
      while(digitalRead(button) == LOW){delay(100);holdcnt++;}
      delay(10);
      if(holdcnt > 2){
        cmd = 3;
        stringComplete = true;
        routineStarted = false;
      }
      holdcnt = 0;
    }
  
    //Serial.println(idlecount);
    if (stringComplete || routineStarted) {
        
    
        // Serial.println("User input:");
        // Serial.println(serialInput);
        // Serial.println("Size:"+String(pktSize));
      	switch (cmd){
    		case(1): 
            if(!(SLEEP_EN==1))
              digitalWrite(onBoardLed, HIGH);
       	  	//Serial.println("ns:"+String(startData.n_s) + "|sd:"+ String(startData.s_d) + "|pd:" + String(startData.p_d));
	          if(trig){
              // Serial.println("WOKE");
              trig = false;
	          }
            prevValue = accelValue;
	          accelValue = pow(pow(accelerationVector.XAxis,2) + pow(accelerationVector.YAxis,2) + pow(accelerationVector.ZAxis,2),0.5);   
	          // Serial.print(accelerationVector.XAxis);
            // Serial.print(", ");
            // Serial.print(accelerationVector.YAxis);
            // Serial.print(", ");
            // Serial.println(accelerationVector.YAxis);
		     
            deltaValue = abs(accelValue - prevValue);
            if(!(THRES_EN==1) || (deltaValue > THRES)){
           	  //Serial.print("dValue ");
           	  Serial.println(deltaValue);
            }

              
	        	break;
	    	case(2):
		        analogWrite(ledPin, 0);
		        for (unsigned int j = 0; j < stopData.brightness; j++) {
		          analogWrite(ledPin, j);
		          delay(stopData.ramptime/stopData.brightness);
		        }
		       	// Serial.println(String(stopData.brightness) + "-"+ String(stopData.ramptime));
	        	break;
	    	case(3):
		       	// Serial.println();
		       	// Serial.println("Sleeping mask turned off");
            if(!(SLEEP_EN==1))
              digitalWrite(onBoardLed, LOW);
     
		        analogWrite(ledPin, 0);
		        cmd = 0;
	        	break;
	    	default:
			// Serial.println("Error");
	        break;
    	}
	    // clear the data:
	    //
	    stringComplete = false;
	    stringStarted = false;
    }
    if(SLEEP_EN){
      idlecount += 1;
    }
    delay(startData.sample_delay);
    
    //accelerationVector = accel.readNormalizeAccel();
    accelerationVector = accel.readNormalizeGyro();
   	if(idlecount >= 200){
   		// Serial.println("Going to sleep now");
      delay(100);
   		sleepNow();
   		// Serial.println("Waking now");
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
		  // Serial.println(inChar, HEX);
	    // add it to the inputString:
	    
	    serialInput[i]= inChar;
	    i++;
	    routineStarted = false;
	   
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
				default:
				  stringStarted = false;
				  break;
	      	}
      }
    
	    // if the incoming character is a newline, set a flag so the main loop can
	    // do something about it:
	    if (i >= pktSize) {
			stringComplete = true;
			switch (cmd){
				case(1): 
				  startData = *((start_pkt *)(serialInput + 1));;
				  routineStarted = true;
				  break;
				case(2):
				  stopData = {255,5000};
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
    case MPU6050_CLOCK_KEEP_RESET:        Serial.println("Stops the clock and keeps the timing generator in reset"); break;
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


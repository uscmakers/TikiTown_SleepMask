
#define MAX_BUFF_SIZE 6

typedef struct __attribute__((packed)){
    unsigned char i_f;
    unsigned int i_t;
    unsigned char s_f;
} start_pkt;

typedef struct __attribute__((packed)){
    unsigned char brightness;
    unsigned char ramptime;
} stop_pkt;

char serialInput[MAX_BUFF_SIZE];
volatile boolean stringComplete = false;  // whether the string is complete
volatile boolean stringStarted = false;
volatile unsigned char cmd = 0;
boolean readdata = false;
volatile int i = 0;
volatile int pktSize = 0;
start_pkt startData;
stop_pkt stopData;



void setup() {
  // initialize serial:
  Serial.begin(9600);
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
//    Serial.println("User input:");
//    Serial.println(serialInput);
//    Serial.println(String(pktSize) + char(97));
    switch (cmd){
      case(1): 
        startData = *((start_pkt *)(serialInput + 1));
        Serial.println(String(startData.i_f) + "-"+ String(startData.i_t) + "-" + String(startData.s_f));
        break;
      case(2):
        stopData = *((stop_pkt *)(serialInput + 1));
        Serial.println(String(stopData.brightness) + "-"+ String(stopData.ramptime));
        break;
      case(3):
        Serial.println("got quit");
        break;
      default:
        Serial.println("Error");
        break;
    }
    // clear the data:
    memset(serialInput, 0, MAX_BUFF_SIZE);
    stringComplete = false;
    stringStarted = false;
    i = 0;
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
    // add it to the inputString:
    
    serialInput[i]= inChar;
    i++;
   
    if (stringStarted == false){
      cmd = inChar;
      stringStarted = true;
      switch (cmd){
        case(1):
          pktSize = 5;
          break;
        case(2):
          pktSize = 3;
          break;
        case(3):
          pktSize = 1;
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
    }
  }
}

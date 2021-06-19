#include <Adafruit_NeoPixel.h>
//#define DEBUG
#ifdef DEBUG
  #define debugPrint(x)  Serial.print (x)
  #define debugPrintln(x)  Serial.println (x)
#else
  #define debugPrint(x)
  #define debugPrintln(x)
#endif

byte brakeLight_runningBrightness = 50;
byte brakeLight_brakeBrightness = 255;
byte brakeLightLEDCount = 40;

byte blinkerInputPin = 2;
byte brakeInputPin = 3;
byte blinkerRelayPin = 6;
byte brakeLightPin = 12;
boolean blinkerRelayState;      //used as a flag to check the blinker
boolean brakeInputState;
boolean blinkerInputState;
unsigned long previousMillis;
unsigned long previousBlinkerMillis;
unsigned long interval = 500;
unsigned long blinkerInterval = 400;

Adafruit_NeoPixel brakeLight(brakeLightLEDCount, brakeLightPin, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(blinkerRelayPin, OUTPUT);
  digitalWrite(blinkerRelayPin, LOW);
  pinMode(blinkerInputPin, INPUT);
  pinMode(brakeInputPin, INPUT);
  Serial.begin(115200);  
  attachInterrupt(digitalPinToInterrupt(brakeInputPin), brakeISR, RISING);    //trigger interrupt when brake line goes high
//  attachInterrupt(digitalPinToInterrupt(blinkerInputPin), blinkerISR, FALLING);   //trigger interrupt when blinker line is low, will need flags
  brakeLight.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  //brakeLight.show();            // Turn OFF all pixels ASAP
  //brakeLight.setBrightness(brakeLight_runningBrightness); // Set BRIGHTNESS (max = 255)
  colorWipe(brakeLight.Color(brakeLight_runningBrightness, 0, 0));
//  brakeLight.show();
}

void loop() {
  if(Serial.available()>0){     //if serial information is available
    byte serialVal = Serial.read();
    if(serialVal == 49){      //the number 1, toggle the relay ckt
      blinkerRelayState = !blinkerRelayState;
      digitalWrite(blinkerRelayPin, blinkerRelayState);
      Serial.print("relay state: "); Serial.println(blinkerRelayState);
    }
  }
  blinkerInputState = digitalRead(blinkerInputPin);
  brakeInputState = digitalRead(brakeInputPin);
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    Serial.print("brake input: "); Serial.print(brakeInputState);
    Serial.print("  blinker input: "); Serial.println(blinkerInputState);
  }  
  if(currentMillis - previousBlinkerMillis >= blinkerInterval){      //handle the blinker relay every blinkerInterval
    previousBlinkerMillis = millis();
    if(blinkerRelayState == 1){
      blinkerRelayState = !blinkerRelayState;
      digitalWrite(blinkerRelayPin, blinkerRelayState);
    }
    else{     //if the relay is open, check the state of the blinker input
      blinkerInputState = digitalRead(blinkerInputPin);
      if(blinkerInputState){     //if the blinker input is high, turn off the blinkers
        blinkerRelayState = 0;
        digitalWrite(blinkerRelayPin, blinkerRelayState);
      }
      else{     //otherwise toggle the blinkers
        blinkerRelayState = !blinkerRelayState;
        digitalWrite(blinkerRelayPin, blinkerRelayState);
      }
    }
  }
  if(brakeInputState){      //if brakes are high, do the brakelight
    colorWipe(brakeLight.Color(brakeLight_brakeBrightness, 0, 0));
  }
  else{                     //if brakes are low, go back to running brightness
    colorWipe(brakeLight.Color(brakeLight_runningBrightness, 0, 0));
  }  
}

void brakeISR(){
  Serial.println("brake ISR triggered");
  if(brakeInputState){      //if brakes are high, do the brakelight
    colorWipe(brakeLight.Color(brakeLight_brakeBrightness, 0, 0));
  }
}

void colorWipe(uint32_t color) {
  for(int i=0; i<brakeLight.numPixels(); i++) { // For each pixel in strip...
    brakeLight.setPixelColor(i, color);         //  Set pixel's color (in RAM)
  }
    brakeLight.show();                          //  Update strip to match
}

//void blinkerISR(){
//  Serial.println("blinker ISR triggered");
//  blinkerFlag = 1;
//}

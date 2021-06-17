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
byte brakeLight_brakeBrightness = 200;
byte brakeLightLEDCount = 25;

byte blinkerInputPin = 2;
byte brakeInputPin = 3;
byte blinkerRelayPin = 6;
byte brakeLightPin = 12;
boolean blinkerRelayState;
boolean brakeInputState;
boolean blinkerInputState;

Adafruit_NeoPixel brakeLight(brakeLightLEDCount, brakeLightPin, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(blinkerRelayPin, OUTPUT);
  digitalWrite(blinkerRelayPin, LOW);
  pinMode(blinkerInputPin, INPUT);
  pinMode(brakeInputPin, INPUT);
  Serial.begin(115200);  
  attachInterrupt(digitalPinToInterrupt(brakeInputPin), brakeISR, RISING);    //trigger interrupt when brake line goes high
  attachInterrupt(digitalPinToInterrupt(blinkerInputPin), blinkerISR, FALLING);   //trigger interrupt when blinker line is low, will need flags
  brakeLight.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  brakeLight.show();            // Turn OFF all pixels ASAP
  brakeLight.setBrightness(brakeLight_runningBrightness); // Set BRIGHTNESS (max = 255)
}

void loop() {
  if(Serial.available()>0){
    byte serialVal = Serial.read();
    if(serialVal == 49){      //the number 1, toggle the relay ckt
      blinkerRelayState = !blinkerRelayState;
      digitalWrite(blinkerRelayPin, blinkerRelayState);
      Serial.print("relay state: "); Serial.println(blinkerRelayState);
    }
  }
  blinkerInputState = digitalRead(blinkerInputPin);
  brakeInputState = digitalRead(brakeInputPin);
  Serial.print("brake input: "); Serial.print(brakeInputState);
  Serial.print("  blinker input: "); Serial.println(blinkerInputState);
  delay(500);
}

void brakeISR(){
  Serial.println("brake ISR triggered");
}

void blinkerISR(){
  Serial.println("blinker ISR triggered");
}

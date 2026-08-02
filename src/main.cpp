#include <Arduino.h>


const byte ledPin = LED_BUILTIN;
const byte interruptPin = 2; // Pin 2 supports interrupts on Uno/Mega
volatile byte state = LOW;   // Must be volatile if changed in ISR
#define OUTPUT_PIN PA7

// This is your Interrupt Service Routine (ISR)
void blinkISR() {
  state = !state;
}

void setup() {
  pinMode(ledPin, OUTPUT);

  ////pinMode(OUTPUT_PIN, OU_PULLUP);
  pinMode(OUTPUT_PIN, OUTPUT);
  //tone(PA8, 1000); 
  // Link the pin to the ISR function
  // attachInterrupt(digitalPinToInterrupt(interruptPin), blinkISR, CHANGE);
   Serial.begin(115200);
}

#define FREQ2_33  2 * 50  //Hz
#define FREQ2_45  2 * 67.65  //Hz

#define FREQ2  FREQ2_33

#define DELAY 1000.0 / (FREQ2)


void loop() {
  static  unsigned cntr = 0;

  delayMicroseconds(DELAY * 1000.0);

  digitalWrite(OUTPUT_PIN, state = !state);

  if ( !(cntr % 10)) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    cntr=0;
  } 
  ++cntr;


  ///Serial.printf("state=%d\n", ret);

}


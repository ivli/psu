#include <Arduino.h>

#if !defined(STM32F411xE)
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#else
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#endif


#include "interrupt.h"


#define PIN_OUTPUT_33_RPM  PB6  // 
#define PIN_OUTPUT_45_RPM  PB7  // 
#define PIN_RPM_BUTTON     PB9  // internal pull-up
#define PIN_REALAY_CONTROL PB12 // drives latching realay 33/45 rpm
#define PIN_LED_33_RPM     PB13 // turns 33 rpm LED on 
#define PIN_LED_45_RPM     PB14 // turns 45 rpm LED on

//for turntable mechanics built for 50Hz mains, for 60Hz mains frequencies shall be corrected
#define RPM_50_MICROSEC  20000 // 50 Hz = 0,2000 s 
#define RPM_45_MICROSEC  14781 // 67.65 Hz = 0,014781 s

HardwareTimer *timSupport = new HardwareTimer(TIM1);
HardwareTimer *timMains = new HardwareTimer(TIM4);

void toggleLED() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

/*
PB6 - TIM4_CH1 (no remap)
PB7 - TIM4_CH2 (no remap)
PB8 - TIM4_CH3 (no remap)
PB9 - TIM4_CH4 (no remap)
*/

void set33()
{
  timMains->pause();
  digitalWrite(PIN_OUTPUT_45_RPM, 0);

  timMains->setMode(1, TIMER_OUTPUT_COMPARE_PWM1, PIN_OUTPUT_33_RPM);
  timMains->setOverflow(RPM_50_MICROSEC, MICROSEC_FORMAT); 

  timMains->setCaptureCompare(1, 50, PERCENT_COMPARE_FORMAT); // 50%
  timMains->setCaptureCompare(2, 0, PERCENT_COMPARE_FORMAT); // 50%

  timMains->refresh();
  timMains->resume();

  timSupport->pause();
  timSupport->setOverflow(1, HERTZ_FORMAT); // blink slow
  timSupport->attachInterrupt(toggleLED);
  timSupport->refresh();
  timSupport->resume();
  
  digitalWrite(PIN_LED_45_RPM, LOW);
  digitalWrite(PIN_LED_33_RPM, HIGH);
}

void set45()
{
  timMains->pause();
  digitalWrite(PIN_OUTPUT_33_RPM, LOW);

  timMains->setMode(2, TIMER_OUTPUT_COMPARE_PWM1, PIN_OUTPUT_45_RPM);
  timMains->setOverflow(RPM_45_MICROSEC, MICROSEC_FORMAT); 
  timMains->setCaptureCompare(2, 50, PERCENT_COMPARE_FORMAT); // 50%
  timMains->setCaptureCompare(1, 0, PERCENT_COMPARE_FORMAT); // 50%

  timMains->refresh();
  timMains->resume();

  timSupport->pause();
  timSupport->setOverflow(2, HERTZ_FORMAT); // blink fast
  timSupport->attachInterrupt(toggleLED);
  timSupport->refresh();
  timSupport->resume();

  digitalWrite(PIN_LED_33_RPM, LOW);
  digitalWrite(PIN_LED_45_RPM, HIGH);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_OUTPUT_33_RPM, OUTPUT);
  pinMode(PIN_OUTPUT_45_RPM, OUTPUT);
  pinMode(PIN_REALAY_CONTROL, OUTPUT);
  pinMode(PIN_LED_33_RPM, OUTPUT);
  pinMode(PIN_LED_45_RPM, OUTPUT);

  pinMode(PIN_RPM_BUTTON, INPUT_PULLUP);
 
  digitalWrite(PIN_REALAY_CONTROL, true);
  set33();

  timSupport->pause();
  timSupport->setOverflow(1, HERTZ_FORMAT); // 50Hz
  timSupport->attachInterrupt(toggleLED);
 //// MyTimLED->setMode(1, TIMER_OUTPUT_DISABLED);
  timSupport->refresh();
  timSupport->resume();
}

static bool is_33 = true;

void loop() {
  if(!digitalRead(PIN_RPM_BUTTON)) {

      do {
        delay(200);
      } while (!digitalRead(PIN_RPM_BUTTON));

      is_33 = !is_33;
      
      digitalWrite(PIN_REALAY_CONTROL,  is_33);

      delay(100);

      if (is_33)
        set33();
      else
        set45(); 
  }
}


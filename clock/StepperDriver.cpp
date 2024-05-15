#include "StepperDriver.h"
#include "settings.h"

void StepperDriver::begin() {
  pinMode(PIN_HOUR_STEP, OUTPUT);
  pinMode(PIN_HOUR_SLEEP, OUTPUT);
  pinMode(PIN_MINUTE_STEP, OUTPUT);
  pinMode(PIN_MINUTE_SLEEP, OUTPUT);

  digitalWrite(PIN_HOUR_STEP, LOW);
  digitalWrite(PIN_HOUR_SLEEP, LOW);
  digitalWrite(PIN_MINUTE_STEP, LOW);
  digitalWrite(PIN_MINUTE_SLEEP, LOW);
}

void StepperDriver::step(bool hour, bool minute) {
  if(!hour && !minute)
    return;

  if(hour)
    digitalWrite(PIN_HOUR_SLEEP, HIGH);

  if(minute)
    digitalWrite(PIN_MINUTE_SLEEP, HIGH);

  delayMicroseconds(2000);

  int hour_steps = hour ? get_hour_step_count() : 0;
  int minute_steps = minute ? get_minute_step_count() : 0;

  int offset = 0;
  while(hour_steps > 0 || minute_steps > 0) {
    offset = (offset + 1) % 60;

    if(offset % 5 == 0) {
      digitalWrite(PIN_MINUTE_STEP, HIGH);
      delayMicroseconds(50);
      digitalWrite(PIN_MINUTE_STEP, LOW);
      minute_steps--;
    }

    if(offset % 12 == 0) {
      digitalWrite(PIN_HOUR_STEP, HIGH);
      delayMicroseconds(50);
      digitalWrite(PIN_HOUR_STEP, LOW);
      hour_steps--;
    }

    delayMicroseconds(30);
  }  

  digitalWrite(PIN_HOUR_SLEEP, LOW);
  digitalWrite(PIN_MINUTE_SLEEP, LOW);  
}

int StepperDriver::get_hour_step_count() {
  static int offset = 0;
  
  int steps = 492;
  offset = (offset + 1) % 20;
  if(offset == 0)
    steps -= 9;

  return steps * MICROSTEPPING_MULTIPLIER;
}

int StepperDriver::get_minute_step_count() {
  static int offset = 0;
  
  int steps = 1180;
  offset = (offset + 1) % 25;
  if(offset == 0)
    steps -= 7;

  return steps * MICROSTEPPING_MULTIPLIER;
}

void StepperDriver::calibrate_hour(bool &cont) {
  digitalWrite(PIN_HOUR_SLEEP, HIGH);
  delayMicroseconds(2000);

  while(cont) {
      digitalWrite(PIN_HOUR_STEP, HIGH);
      delayMicroseconds(50);
      digitalWrite(PIN_HOUR_STEP, LOW);
      delayMicroseconds(1000);
  }

  digitalWrite(PIN_HOUR_SLEEP, LOW);
}

void StepperDriver::calibrate_minute(bool &cont) {
  digitalWrite(PIN_MINUTE_SLEEP, HIGH);
  delayMicroseconds(2000);

  while(cont) {
      digitalWrite(PIN_MINUTE_STEP, HIGH);
      delayMicroseconds(50);
      digitalWrite(PIN_MINUTE_STEP, LOW);
      delayMicroseconds(1000);
  }

  digitalWrite(PIN_MINUTE_SLEEP, LOW);
}
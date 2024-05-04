#include "StepperDriver.h"

void StepperDriver::begin() {
  pinMode(HOUR_STEP_PIN, OUTPUT);
  pinMode(HOUR_SLEEP_PIN, OUTPUT);
  pinMode(MINUTE_STEP_PIN, OUTPUT);
  pinMode(MINUTE_SLEEP_PIN, OUTPUT);

  digitalWrite(HOUR_STEP_PIN, LOW);
  digitalWrite(HOUR_SLEEP_PIN, LOW);
  digitalWrite(MINUTE_STEP_PIN, LOW);
  digitalWrite(MINUTE_SLEEP_PIN, LOW);
}

void StepperDriver::step(bool hour, bool minute) {
  if(!hour && !minute)
    return;

  if(hour)
    digitalWrite(HOUR_SLEEP_PIN, HIGH);

  if(minute)
    digitalWrite(MINUTE_SLEEP_PIN, HIGH);

  delayMicroseconds(2000);

  int hour_steps = hour ? get_hour_step_count() : 0;
  int minute_steps = minute ? get_minute_step_count() : 0;

  int offset = 0;
  while(hour_steps > 0 || minute_steps > 0) {
    offset = (offset + 1) % 60;

    if(offset % 5 == 0) {
      digitalWrite(MINUTE_STEP_PIN, HIGH);
      delayMicroseconds(50);
      digitalWrite(MINUTE_STEP_PIN, LOW);
      minute_steps--;
    }

    if(offset % 12 == 0) {
      digitalWrite(HOUR_STEP_PIN, HIGH);
      delayMicroseconds(50);
      digitalWrite(HOUR_STEP_PIN, LOW);
      hour_steps--;
    }

    delayMicroseconds(30);
  }  

  digitalWrite(HOUR_SLEEP_PIN, LOW);
  digitalWrite(MINUTE_SLEEP_PIN, LOW);  
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
  digitalWrite(HOUR_SLEEP_PIN, HIGH);
  delayMicroseconds(2000);

  while(cont) {
      digitalWrite(HOUR_STEP_PIN, HIGH);
      delayMicroseconds(50);
      digitalWrite(HOUR_STEP_PIN, LOW);
      delayMicroseconds(1000);
  }

  digitalWrite(HOUR_SLEEP_PIN, LOW);
}

void StepperDriver::calibrate_minute(bool &cont) {
  digitalWrite(MINUTE_SLEEP_PIN, HIGH);
  delayMicroseconds(2000);

  while(cont) {
      digitalWrite(MINUTE_STEP_PIN, HIGH);
      delayMicroseconds(50);
      digitalWrite(MINUTE_STEP_PIN, LOW);
      delayMicroseconds(1000);
  }

  digitalWrite(MINUTE_SLEEP_PIN, LOW);
}
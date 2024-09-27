#include "StepperDriver.h"
#include "settings.h"

void StepperDriver::begin() {
  pinMode(PIN_HOUR_STEP, OUTPUT);
  pinMode(PIN_HOUR_SLEEP, OUTPUT);
  pinMode(PIN_MINUTE_STEP, OUTPUT);
  pinMode(PIN_MINUTE_SLEEP, OUTPUT);
  pinMode(PIN_DRV8825, OUTPUT);

  digitalWrite(PIN_HOUR_STEP, LOW);
  digitalWrite(PIN_HOUR_SLEEP, LOW);
  digitalWrite(PIN_MINUTE_STEP, LOW);
  digitalWrite(PIN_MINUTE_SLEEP, LOW);

#if USE_TMC2208
  digitalWrite(PIN_DRV8825, LOW);
#else
  digitalWrite(PIN_DRV8825, HIGH);
#endif
}

void StepperDriver::step(bool hour, bool minute, int stepsDone, int stepGroup) {
  if(!hour && !minute)
    return;

#if !USE_TMC2208
  if(hour)
    digitalWrite(PIN_HOUR_SLEEP, HIGH);

  if(minute)
    digitalWrite(PIN_MINUTE_SLEEP, HIGH);
#endif

  delayMicroseconds(2000);

  int hour_steps = hour ? get_hour_step_count() : 0;
  int minute_steps = minute ? get_minute_step_count() : 0;

  int step_target = hour_steps > minute_steps ? hour_steps : minute_steps; 

  int offset = 0;
  int last_progress = -360;
  while(hour_steps > 0 || minute_steps > 0) {
    offset = (offset + 1) % 60;

    if(offset % 5 == 0 && minute_steps > 0) {
      digitalWrite(PIN_MINUTE_STEP, HIGH);
      delayMicroseconds(5);
      digitalWrite(PIN_MINUTE_STEP, LOW);
      minute_steps--;
    }

    if(offset % 12 == 0 && hour_steps > 0) {
      digitalWrite(PIN_HOUR_STEP, HIGH);
      delayMicroseconds(5);
      digitalWrite(PIN_HOUR_STEP, LOW);
      hour_steps--;
    }

    if(offset == 0) {
      int steps_remaining = hour_steps > minute_steps ? hour_steps : minute_steps;
      int progress = (360 * ((step_target - steps_remaining) + (stepsDone * step_target))) / (stepGroup * step_target);

      if(progress >= last_progress + 3 && progress > 0) {
        last_progress = progress;
        _seconds_display->show_progress(progress);
      }
    }
    
    delayMicroseconds(STEP_INTERVAL);    
  }

#if !USE_TMC2208
  digitalWrite(PIN_HOUR_SLEEP, LOW);
  digitalWrite(PIN_MINUTE_SLEEP, LOW);  
#endif
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
#if !USE_TMC2208
  digitalWrite(PIN_HOUR_SLEEP, HIGH);
  delayMicroseconds(2000);
#endif

  while(cont) {
      digitalWrite(PIN_HOUR_STEP, HIGH);
      delayMicroseconds(5);
      digitalWrite(PIN_HOUR_STEP, LOW);
      delayMicroseconds(4 * 12 * STEP_INTERVAL);
  }

#if !USE_TMC2208
  digitalWrite(PIN_HOUR_SLEEP, LOW);
#endif
}

void StepperDriver::calibrate_minute(bool &cont) {
#if !USE_TMC2208
  digitalWrite(PIN_MINUTE_SLEEP, HIGH);
  delayMicroseconds(2000);
#endif

  while(cont) {
      digitalWrite(PIN_MINUTE_STEP, HIGH);
      delayMicroseconds(5);
      digitalWrite(PIN_MINUTE_STEP, LOW);
      delayMicroseconds(4 * 5 * STEP_INTERVAL);
  }

#if !USE_TMC2208
  digitalWrite(PIN_MINUTE_SLEEP, LOW);
#endif
}
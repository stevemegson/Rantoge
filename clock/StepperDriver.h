#ifndef STEPPERDRIVER_H
#define STEPPERDRIVER_H

#include <Arduino.h>
#include "SecondsDisplay.h"

class StepperDriver {
public:
  void begin();

  void step(bool hour, bool minute, int stepsDone = 0, int stepGroup = 1);
  
  void step_hour(int steps);
  void step_minute(int steps);
  void calibrate_hour(bool &cont);
  void calibrate_minute(bool &cont);

  void set_seconds_display(SecondsDisplay *seconds) {
    _seconds_display = seconds;
  }

private:
  int get_hour_step_count();
  int get_minute_step_count();
  SecondsDisplay *_seconds_display;
  
};

#endif
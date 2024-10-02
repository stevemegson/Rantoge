#ifndef STEPPERDRIVER_H
#define STEPPERDRIVER_H

#include <Arduino.h>
#include "settings.h"

#if ENABLE_TFT == 1
#include "SecondsDisplay.h"
#endif

class StepperDriver {
public:
  void begin();

  void step(bool hour, bool minute, int stepsDone = 0, int stepGroup = 1);
  
  void step_hour(int steps);
  void step_minute(int steps);
  void calibrate_hour(bool &cont);
  void calibrate_minute(bool &cont);

  void set_12_hour(bool is_12_hour);
#if ENABLE_TFT == 1
  void set_seconds_display(SecondsDisplay *seconds) {
    _seconds_display = seconds;
  }
#endif

private:
  int get_hour_step_count();
  int get_minute_step_count();

  bool _is_12_hour;

#if ENABLE_TFT == 1
  SecondsDisplay *_seconds_display;
#endif  
};

#endif
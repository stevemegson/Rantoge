#ifndef STEPPERDRIVER_H
#define STEPPERDRIVER_H

#include <Arduino.h>

class StepperDriver {
public:
  void begin();

  void step(bool hour, bool minute);

  void step_hour(int steps);
  void step_minute(int steps);
  void calibrate_hour(bool &cont);
  void calibrate_minute(bool &cont);

private:
  int get_hour_step_count();
  int get_minute_step_count();
};

#endif
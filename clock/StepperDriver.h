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
  const int HOUR_STEP_PIN =  21;
  const int HOUR_SLEEP_PIN =  22;
  const int MINUTE_STEP_PIN =  18;
  const int MINUTE_SLEEP_PIN =  19;

  const int MICROSTEPPING_MULTIPLIER = 32;

  int get_hour_step_count();
  int get_minute_step_count();
};

#endif
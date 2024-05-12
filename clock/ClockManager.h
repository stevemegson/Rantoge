#ifndef CLOCKMANAGER_H
#define CLOCKMANAGER_H

#include "StepperDriver.h"

typedef void (*logger_cb_t)(const char* format, ...);

class ClockManager {
public:
  void tick();
  void start_ntp();
  void set_displayed_time(int hour, int minute);
  void set_current_time(int hour, int minute, int second);
  void set_current_date(int day, int month, int year);
  void set_logger(logger_cb_t logger);

  void request_calibrate_hour();
  void request_calibrate_minute();
  void request_end_calibrate();
  void toggle_demo();

private:
  enum state_t { RUN,
                 CALIBRATE_HOUR,
                 CALIBRATE_MINUTE,
                 DEMO };

  bool _calibrating = false;
  int _displayedHour = 0;
  int _displayedMinute = 0;
  state_t _state = RUN;

  StepperDriver _stepper;
  logger_cb_t _logger;

  void log_current_time();
};

#endif
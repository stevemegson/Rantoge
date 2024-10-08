#ifndef CLOCKMANAGER_H
#define CLOCKMANAGER_H

#include <Preferences.h>
#include "settings.h"
#include "StepperDriver.h"

#if ENABLE_TFT == 1
#include "SecondsDisplay.h"
#endif

typedef void (*logger_cb_t)(const char* format, ...);

enum time_source_t {
  NONE,
  SNTP,
  WEB,
  BUTTONS,
  GPS
};

enum clock_mode_t {
  MODE_24 = 0,
  MODE_12 = 1,
  MODE_SIMULATE12 = 2
};

static const char *CLOCK_MODE_STRING[] = {
    "24 hour", "12 hour", "simulated 12 hour"
};

class ClockManager {
public:
  time_source_t time_source = NONE;

  void begin();
  void tick();
  void start_ntp();
  void set_displayed_time(int hour, int minute);
  void set_current_time(int hour, int minute, int second);
  void set_current_date(int day, int month, int year);
  void set_displayed_time_to_current();
  void set_mode(clock_mode_t mode);
  void toggle_demo();

  void set_logger(logger_cb_t logger) {
    _logger = logger;
  }

  void request_calibrate_hour() {
    _state = CALIBRATE_HOUR;
  }

  void request_calibrate_minute() {
    _state = CALIBRATE_MINUTE;
  }
  
  void request_end_calibrate() {
    _calibrating = false;
    _state = RUN;
  }

  void request_set_minutes() {
    _state = SET_MINUTES;
  }

  void increment_hour() {
    adjust_displayed_hour(-1);
  }

  void decrement_hour() {
    adjust_displayed_hour(1);
  }

  void increment_minute() {
    if (_state == SET_MINUTES)
      _flagCurrentMinute = true;
    else
      adjust_displayed_minute(-1);
  }

  void decrement_minute() {
    if (_state == SET_MINUTES)
      _flagZeroMinute = true;
    else
      adjust_displayed_minute(1);
  }

private:
  enum state_t { RUN,
                 CALIBRATE_HOUR,
                 CALIBRATE_MINUTE,
                 DEMO,
                 SET_MINUTES };

  bool _calibrating = false;
  int _displayedHour = 0;
  int _displayedMinute = 0;
  state_t _state = RUN;
  bool _flagCurrentMinute;
  bool _flagZeroMinute;
  clock_mode_t _mode;

  Preferences _preferences;
  StepperDriver _stepper;

#if ENABLE_TFT == 1  
  SecondsDisplay _seconds_display;
#endif

  logger_cb_t _logger;

  void sync_to_current_time();
  void log_current_time();
  void set_minutes();
  void adjust_displayed_hour(int count);
  void adjust_displayed_minute(int count);
};

#endif
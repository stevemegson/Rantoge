#include <Arduino.h>

#include "ClockManager.h"

void ClockManager::begin() {
  _stepper.begin();
#if ENABLE_TFT == 1
  _seconds_display.begin();
  _stepper.set_seconds_display(&_seconds_display);
#endif
}

void ClockManager::start_ntp() {
  configTime(0, 0, "pool.ntp.org");

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    (*_logger)("Failed to obtain time");
    return;
  }

  time_source = SNTP;

  set_rgb(0,255,0);

  log_current_time();
  set_displayed_time_to_current();
  _state = RUN;
}

void ClockManager::tick() {
  if (_state == CALIBRATE_HOUR) {
    (*_logger)("Calibrate hour");
    set_rgb(255, 0, 255);

    _calibrating = true;
    _stepper.calibrate_hour(_calibrating);

    (*_logger)("Calibration done");
    set_rgb(0, 0, 0);

    return;
  }

  if (_state == CALIBRATE_MINUTE) {
    (*_logger)("Calibrate minute");
    set_rgb(255, 0, 255);

    _calibrating = true;
    _stepper.calibrate_minute(_calibrating);

    (*_logger)("Calibration done");
    set_rgb(0, 0, 0);

    return;
  }

  if (_state == DEMO) {
    _stepper.step(true, false);
    delay(500);
    _stepper.step(true, true);
    delay(500);
    _stepper.step(false, true);
    delay(500);
    
    adjust_displayed_hour(2);
    adjust_displayed_minute(2);

    return;
  }

  if (_state == SET_MINUTES) {    
    set_rgb(0, 255, 0);
    set_minutes();
    set_rgb(0, 0, 0);
    _state = RUN;

    return;
  }

  sync_to_current_time();
}

void ClockManager::toggle_demo() {
  _calibrating = false;
  if (_state == DEMO) {
    (*_logger)("End demo");
    set_rgb(0, 0, 0);
    _state = RUN;
  } else {
    (*_logger)("Start demo");
    set_rgb(0, 255, 255);
    _state = DEMO;
  }
}

void ClockManager::set_displayed_time(int hour, int minute) {
  _displayedHour = hour;
  _displayedMinute = minute;

  (*_logger)("Set displayed time to %02d:%02d\n", _displayedHour, _displayedMinute);
}

void ClockManager::set_displayed_time_to_current() {
  struct tm timeinfo;
  time_t now = time(0);
  localtime_r(&now, &timeinfo);
  int currentHour = timeinfo.tm_hour;
  int currentMinute = timeinfo.tm_min;

#if SIMULATE_12_HOUR == 1
  if (currentHour == 0) {
    currentHour = 12;
  } else if (currentHour > 12) {
    currentHour -= 12;
  }
#endif

  set_displayed_time(currentHour, currentMinute);
}

void ClockManager::adjust_displayed_hour(int count) {
  _displayedHour = _displayedHour + count;
  if (_displayedHour < 0)
    _displayedHour += 24;
  else if (_displayedHour >= 24)
    _displayedHour -= 24;
}

void ClockManager::adjust_displayed_minute(int count) {
  _displayedMinute = _displayedMinute + count;
  if (_displayedMinute < 0)
    _displayedMinute += 60;
  else if (_displayedMinute >= 60)
    _displayedMinute -= 60;
}

void ClockManager::set_current_time(int hour, int minute, int second) {
  struct tm timeinfo;
  time_t now = time(0);
  localtime_r(&now, &timeinfo);

  timeinfo.tm_hour = hour;
  timeinfo.tm_min = minute;
  timeinfo.tm_sec = second;
  timeinfo.tm_isdst = -1;

  time_t t = mktime(&timeinfo);
  struct timeval new_time = { .tv_sec = t };
  settimeofday(&new_time, NULL);

  log_current_time();
}

void ClockManager::set_current_date(int day, int month, int year) {
  struct tm timeinfo;
  time_t now = time(0);
  localtime_r(&now, &timeinfo);

  timeinfo.tm_mday = day;
  timeinfo.tm_mon = month - 1;
  timeinfo.tm_year = year - 1900;
  timeinfo.tm_isdst = -1;

  time_t t = mktime(&timeinfo);
  struct timeval new_time = { .tv_sec = t };
  settimeofday(&new_time, NULL);

  log_current_time();
}

void ClockManager::log_current_time() {
  struct tm timeinfo;
  time_t now = time(0);
  localtime_r(&now, &timeinfo);
  char buffer[32];
  strftime(buffer, 31, "%Y-%m-%d %H:%M:%S %z", &timeinfo);
  (*_logger)("Current time is %s", buffer);
}

void ClockManager::sync_to_current_time() {
  struct tm timeinfo;
  time_t now = time(0);
  localtime_r(&now, &timeinfo);

  int currentHour = timeinfo.tm_hour;
  int currentMinute = timeinfo.tm_min;
  int currentSecond = timeinfo.tm_sec;

#if SIMULATE_12_HOUR == 1
  if (currentHour == 0) {
    currentHour = 12;
  } else if (currentHour > 12) {
    currentHour -= 12;
  }
#endif

  bool waiting = false;

  int offsetHour = currentHour - _displayedHour;
  if (offsetHour < 0)
    offsetHour += 24;
  else if (offsetHour >= 24)
    offsetHour -= 24;

  int offsetMinute = currentMinute - _displayedMinute;
  if (offsetMinute < 0)
    offsetMinute += 60;
  else if (offsetMinute >= 60)
    offsetMinute -= 60;

  if (offsetMinute > 50) {
    offsetMinute = 0;
    waiting = true;
  }

  if(offsetHour == 23 && currentMinute >= 50) {
    offsetHour = 0;
    waiting = true;
  }

#if ENABLE_TFT == 1
  _seconds_display.update(currentMinute, currentSecond, waiting);
#endif

  if(waiting) {
      set_rgb(255,0,0);
  }

  if(offsetHour == 0 && offsetMinute == 0) {
    return;
  }

  set_rgb(0,0,0);

  (*_logger)("%02d:%02d -> %02d:%02d", _displayedHour, _displayedMinute, currentHour, currentMinute);

  int stepGroup = offsetHour > offsetMinute ? offsetHour : offsetMinute;
  int stepsDone = 0;

  while (offsetHour > 0 && offsetMinute > 0) {
    (*_logger)("  Advance hour and minute");
    offsetHour--;
    adjust_displayed_hour(1);
    offsetMinute--;
    adjust_displayed_minute(1);

    _stepper.step(true, true, stepsDone, stepGroup);
    ++stepsDone;
  }

  while (offsetHour > 0) {
    (*_logger)("  Advance hour");
    offsetHour--;
    adjust_displayed_hour(1);

    _stepper.step(true, false, stepsDone, stepGroup);
    ++stepsDone;
  }

  while (offsetMinute > 0) {
    (*_logger)("  Advance minute");
    offsetMinute--;
    adjust_displayed_minute(1);

    _stepper.step(false, true, stepsDone, stepGroup);
    ++stepsDone;
  }
}

void ClockManager::set_minutes() {
  (*_logger)("Setting minutes");

  _flagCurrentMinute = false;
  _flagZeroMinute = false;

  int currentIndex = -1;
  int zeroIndex = -1;
  bool timeKnown = (time_source != NONE && time_source != BUTTONS);

  unsigned long startMillis = millis();
  int currentMinuteAtStart = 0;
  if (timeKnown) {
    struct tm timeinfo;
    time_t now = time(0);
    localtime_r(&now, &timeinfo);      
    currentMinuteAtStart = timeinfo.tm_min;
  }

  for (int index = 1; index <= 60; index++) {
    _stepper.step(false, true);
    adjust_displayed_minute(1);

    (*_logger)("  Step %02d displayed=%02d zero=%02d current=%02d", index, _displayedMinute, zeroIndex, currentIndex);

    if (_flagZeroMinute) {
      _flagZeroMinute = false;
      zeroIndex = index;
      _displayedMinute = 0;
      if (timeKnown || currentIndex >= 0)
        break;
    }
    if (_flagCurrentMinute) {
      _flagCurrentMinute = false;
      currentIndex = index;
      if (zeroIndex < 0)
        _displayedMinute = currentMinuteAtStart;

      if (timeKnown || zeroIndex >= 0)
        break;
    }
  }

  (*_logger)("          displayed=%02d zero=%02d current=%02d", _displayedMinute, zeroIndex, currentIndex);

  if (!timeKnown) {
    int elapsedMinutes = (millis() - startMillis) / (60 * 1000);
    int currentMinute = currentIndex - zeroIndex + elapsedMinutes;

    if (currentMinute < 0)
      currentMinute += 60;

    time_source = BUTTONS;
    set_current_time(0, currentMinute, 0);
  }
}
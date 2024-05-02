#include <Arduino.h>

#include "ClockManager.h"

void ClockManager::set_logger(logger_cb_t logger) {
  _logger = logger;
}

void ClockManager::start_ntp() {
  configTime(0, 0, "pool.ntp.org");

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    (*_logger)("Failed to obtain time");
    return;
  }

  log_current_time();
  set_displayed_time(timeinfo.tm_hour, timeinfo.tm_min);
  _state = RUN;
}

void ClockManager::tick() {
  if (_state == CALIBRATE_HOUR) {
    (*_logger)("Calibrate hour");

    _calibrating = true;
    _stepper.calibrate_hour(_calibrating);

    (*_logger)("Calibration done");

    return;
  }

  if (_state == CALIBRATE_MINUTE) {
    (*_logger)("Calibrate minute");

    _calibrating = true;
    _stepper.calibrate_minute(_calibrating);

    (*_logger)("Calibration done");

    return;
  }

  struct tm currentTime;
  if (!getLocalTime(&currentTime)) {
    return;
  }

  int currentHour = currentTime.tm_hour;
  int currentMinute = currentTime.tm_min;

  if (currentHour == _displayedHour && currentMinute == _displayedMinute) {
    return;
  }

  (*_logger)("%02d:%02d -> %02d:%02d\n", _displayedHour, _displayedMinute, currentHour, currentMinute);

  int offsetHour = (currentHour - _displayedHour) % 24;
  if (offsetHour < 0) {
    offsetHour += 24;
  }

  int offsetMinute = (currentMinute - _displayedMinute) % 60;
  if (offsetMinute < 0) {
    offsetMinute += 60;
  }

  if (offsetMinute > 50) {
    offsetMinute = 0;
  }

  while (offsetHour > 0 && offsetMinute > 0) {
    (*_logger)("  Advance hour and minute");
    offsetHour--;
    _displayedHour = (_displayedHour + 1) % 24;
    offsetMinute--;
    _displayedMinute = (_displayedMinute + 1) % 60;

    _stepper.step(true, true);
  }

  while (offsetHour > 0) {
    (*_logger)("  Advance hour");
    offsetHour--;
    _displayedHour = (_displayedHour + 1) % 24;

    _stepper.step(true, false);
  }

  while (offsetMinute > 0) {
    (*_logger)("  Advance minute");
    offsetMinute--;
    _displayedMinute = (_displayedMinute + 1) % 60;

    _stepper.step(false, true);
  }
}

void ClockManager::request_calibrate_hour() {
  _state = CALIBRATE_HOUR;
}

void ClockManager::request_calibrate_minute() {
  _state = CALIBRATE_MINUTE;
}

void ClockManager::request_end_calibrate() {
  _calibrating = false;
  _state = RUN;
}

void ClockManager::set_displayed_time(int hour, int minute) {
  _displayedHour = hour;
  _displayedMinute = minute;

  (*_logger)("Set displayed time to %02d:%02d\n", _displayedHour, _displayedMinute);
}

void ClockManager::set_current_time(int hour, int minute, int second) {
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  timeinfo.tm_hour = hour;
  timeinfo.tm_min = minute;
  timeinfo.tm_sec = second;
  timeinfo.tm_isdst = -1;

  time_t timeval = mktime(&timeinfo);
  time(&timeval);

  log_current_time();
}

void ClockManager::set_current_date(int day, int month, int year) {
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  timeinfo.tm_mday = day;
  timeinfo.tm_mon = month - 1;
  timeinfo.tm_year = year - 1900;
  timeinfo.tm_isdst = -1;

  time_t timeval = mktime(&timeinfo);
  time(&timeval);

  log_current_time();
}

void ClockManager::log_current_time() {
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  char buffer[32];
  strftime(buffer, 31, "%Y-%m-%d %H:%M:%S %z", &timeinfo);
  (*_logger)("Current time is %s", buffer);
}
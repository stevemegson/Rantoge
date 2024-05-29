#include "GPS.h"
#include "settings.h"

GpsTimeSource::GpsTimeSource(ClockManager* clock)
  : _serial(GPS_HARDWARE_SERIAL) {
  _clock_manager = clock;
}

void GpsTimeSource::begin() {
  _serial.begin(GPS_BAUD);
}

void GpsTimeSource::tick() {
  while (_serial.available()) {
    if (_gps.encode(_serial.read())) {

      if (_gps.date.isUpdated() && _gps.time.isUpdated()) {
        struct tm tt = {
          .tm_sec = _gps.time.second(),
          .tm_min = _gps.time.minute(),
          .tm_hour = _gps.time.hour(),
          .tm_mday = _gps.date.day(),
          .tm_mon = _gps.date.month() - 1,
          .tm_year = _gps.date.year() - 1900,
          .tm_isdst = 0
        };

        // Convert UTC time in tt to a time_t
        time_t tLoc = mktime(&tt);
        tt = *gmtime(&tLoc);
        tt.tm_isdst = 0;
        time_t tRev = mktime(&tt);
        time_t tDiff = tLoc - tRev;
        time_t tUTC = tLoc + tDiff;

        time(&tUTC);

        _clock_manager->time_source = GPS;
      }
    }
  }
}

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "TimeZoneManager.h"
#include "settings.h"

Preferences preferences;

void TimeZoneManager::set_logger(logger_cb_t logger) {
  _logger = logger;
}


void TimeZoneManager::begin(ClockManager *clock) {
  _preferences.begin("time", false);
  String tz = _preferences.getString("tz", "");
  _preferences.end();

  if (tz != "") {
    setenv("TZ", tz.c_str(), 1);
    tzset();
    (*_logger)("Loaded TZ = %s", tz.c_str());
  } else {
    if (WiFi.status() == WL_CONNECTED) {
      set_from_api();
    }
  }

  clock->set_displayed_time_to_current();
}

void TimeZoneManager::set(String tz) {
  const char* posix = convert_name(tz.c_str());
  setenv("TZ", posix, 1);
  tzset();

  _preferences.begin("time", false);
  _preferences.putString("tz", String(posix));
  _preferences.end();

  (*_logger)("Set time zone to %s", tz);
  (*_logger)("TZ = %s", posix);
}

void TimeZoneManager::set_from_api() {
  #if ENABLE_WIFI == 1
  WiFiClient client;
  HTTPClient http;

  (*_logger)("Requesting time zone from ip-api.com");

  http.begin(client, "http://ip-api.com/line/?fields=256");
  int httpResponseCode = http.GET();

  String payload = "";
  if (httpResponseCode > 0) {
    payload = http.getString();
    payload.trim();
    set(payload);
  } else {
    (*_logger)("Error code: %s", httpResponseCode);
  }
  http.end();
  #endif
}

const char* TimeZoneManager::convert_name(const char* tzdata) {
  int offset = 0;
  const char* current = TIME_ZONE_NAMES;
  while (strlen(current) > 0) {
    if (strcmp(tzdata, current) == 0) {
      const char* posix = TIME_ZONE_STRINGS;
      while (offset > 0 || strcmp(posix, "~") == 0) {
        posix = posix + strlen(posix) + 1;
        offset--;
      }
      return posix;
    }

    current = current + strlen(current) + 1;
    offset++;
  }

  return "UTC0";
}
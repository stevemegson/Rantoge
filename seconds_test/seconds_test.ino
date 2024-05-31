// Uses TFT_eSPI library
// See pin assignments in tft_settings.h

#include "SecondsDisplay.h"

SecondsDisplay seconds_display;

void setup() {
  // 1 Jan 1970 is as good a time as any to test
  struct timeval new_time = { .tv_sec = 0 };
  settimeofday(&new_time, NULL);

  seconds_display.begin();
}


void loop() {

  struct tm timeinfo;
  time_t now = time(0);
  localtime_r(&now, &timeinfo);

  int currentMinute = timeinfo.tm_min;
  int currentSecond = timeinfo.tm_sec;

  seconds_display.update(currentMinute, currentSecond);

  delay(100);
}

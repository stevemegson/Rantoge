#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include "ClockManager.h"

class GpsTimeSource {
public:
  GpsTimeSource(ClockManager* clock_manager);
  void begin();
  void tick();

private:
  HardwareSerial _serial;
  TinyGPSPlus _gps;
  ClockManager* _clock_manager;
};

#endif
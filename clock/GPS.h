#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include "ClockManager.h"

// TX 17, RX 16
#define HARDWARE_SERIAL 2
#define BAUD 9600

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
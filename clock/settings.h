#ifndef SETTINGS_H
#define SETTINGS_H

// ---- FEATURES -------------------
#define ENABLE_WIFI 1
#define ENABLE_SNTP 1
#define ENABLE_BUTTONS 1
#define ENABLE_OTA 1
#define ENABLE_GPS 0
#define ENABLE_TFT 0
#define ENABLE_RGB 1

// ---- PIN MAPPING ----------------
#if CONFIG_IDF_TARGET_ESP32

  #define PIN_DRV8825      GPIO_NUM_16

  #define PIN_LEFT_BUTTON  GPIO_NUM_26
  #define PIN_RIGHT_BUTTON GPIO_NUM_23

  #define PIN_HOUR_STEP    GPIO_NUM_21
  #define PIN_HOUR_SLEEP   GPIO_NUM_22
  #define PIN_MINUTE_STEP  GPIO_NUM_18
  #define PIN_MINUTE_SLEEP GPIO_NUM_19

#elif CONFIG_IDF_TARGET_ESP32S3

  #define PIN_DRV8825      GPIO_NUM_8

  #define PIN_LEFT_BUTTON  GPIO_NUM_2
  #define PIN_RIGHT_BUTTON GPIO_NUM_1
  #define PIN_THIRD_BUTTON GPIO_NUM_7

  #define PIN_HOUR_STEP    GPIO_NUM_6
  #define PIN_HOUR_SLEEP   GPIO_NUM_5
  #define PIN_MINUTE_STEP  GPIO_NUM_4
  #define PIN_MINUTE_SLEEP GPIO_NUM_3

  #define PIN_RGB          GPIO_NUM_21

#elif CONFIG_IDF_TARGET_ESP32C3

  #define PIN_DRV8825      GPIO_NUM_7

  #define PIN_LEFT_BUTTON  GPIO_NUM_1
  #define PIN_RIGHT_BUTTON GPIO_NUM_0
  #define PIN_THIRD_BUTTON GPIO_NUM_6

  #define PIN_HOUR_STEP    GPIO_NUM_5
  #define PIN_HOUR_SLEEP   GPIO_NUM_4
  #define PIN_MINUTE_STEP  GPIO_NUM_3
  #define PIN_MINUTE_SLEEP GPIO_NUM_2

  #define PIN_RGB          GPIO_NUM_0
#endif

// ---- OPTIONS --------------------
#define USE_TMC2208 0
#define SIMULATE_12_HOUR 0
#define STEP_DELAY 15
#define MICROSTEPPING_MULTIPLIER 32

#define GPS_HARDWARE_SERIAL 2 // TX 17, RX 16
#define GPS_BAUD 9600

// ---------------------------------
const int STEP_INTERVAL = STEP_DELAY * 32 / MICROSTEPPING_MULTIPLIER;

void set_rgb(uint8_t r, uint8_t g, uint8_t b);

#endif
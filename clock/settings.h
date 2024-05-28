// ---- FEATURES -------------------


#define ENABLE_WIFI 1
#define ENABLE_SNTP 1
#define ENABLE_BUTTONS 1
#define ENABLE_OTA 1
#define ENABLE_GPS 1

// ---- PIN MAPPING ----------------

#define PIN_LEFT_BUTTON  GPIO_NUM_26
#define PIN_RIGHT_BUTTON GPIO_NUM_23

#define PIN_HOUR_STEP    GPIO_NUM_21
#define PIN_HOUR_SLEEP   GPIO_NUM_22
#define PIN_MINUTE_STEP  GPIO_NUM_18
#define PIN_MINUTE_SLEEP GPIO_NUM_19

#define MICROSTEPPING_MULTIPLIER 32
#define STEP_INTERVAL 15

// ---- OPTIONS --------------------

#define SIMULATE_12_HOUR 0

// ---------------------------------

#define GPS_HARDWARE_SERIAL 2 // TX 17, RX 16
#define GPS_BAUD 9600

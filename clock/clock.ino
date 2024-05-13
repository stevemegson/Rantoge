#define ENABLE_WIFI 1
#define ENABLE_SNTP 1
#define ENABLE_BUTTONS 1
#define ENABLE_GPS 0

#if ENABLE_BUTTONS == 1
#include <espasyncbutton.hpp>
#endif

#if ENABLE_WIFI == 1
#include <ESPAsyncWebServer.h>
#include "WifiManager.h"
#include "HTML.h"
#endif

#if ENABLE_GPS == 1
#include "GPS.h"
#endif

#include <Preferences.h>
#include <time.h>
#include <sys/time.h>

#include "ClockManager.h"
#include "TimeZoneManager.h"

ClockManager clock_manager;
TimeZoneManager time_zone_manager;

#if ENABLE_BUTTONS == 1
AsyncEventButton left_button(GPIO_NUM_26, LOW);
AsyncEventButton right_button(GPIO_NUM_23, LOW);
#endif

#if ENABLE_WIFI == 1
AsyncWebServer server(80);
AsyncEventSource events("/events");
WifiManager wifi_manager;
#endif

#if ENABLE_GPS == 1
GpsTimeSource gps(&clock_manager);
#endif

typedef void (*logger_cb_t) (const char * format, ...);

void setup() {
  Serial.begin(115200);
  delay(1000);

#if ENABLE_BUTTONS == 1
  start_buttons();
#endif

#if ENABLE_WIFI == 1
  wifi_manager.begin();
  wifi_manager.start_mdns("clock");

  start_server();
#endif  

#if ENABLE_WIFI == 1 && ENABLE_SNTP == 1
  clock_manager.start_ntp(); 
#endif

  clock_manager.set_logger(send_message);

  time_zone_manager.set_logger(send_message);
  time_zone_manager.begin(&clock_manager);

#if ENABLE_GPS == 1
  gps.begin();
#endif  
}

void loop() {
#if ENABLE_GPS == 1
  gps.tick();
#endif

  clock_manager.tick();

  delay(1000);
}

void send_message(const char* format, ...)
{
  char buffer[256];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, 255, format, args);
  va_end (args);

  Serial.println(buffer);
  
#if ENABLE_WIFI == 1  
  events.send(buffer, "message", millis());
#endif
}


#if ENABLE_WIFI == 1
void start_server() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", CONFIG_HTML);
  });

  server.on("/set-displayed", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("h", true) && request->hasParam("m", true)) {
      int h = request->getParam("h", true)->value().toInt();
      int m = request->getParam("m", true)->value().toInt();

      clock_manager.set_displayed_time(h, m);
      request->send_P(200, "text/plain", "OK");

    } else {
      request->send_P(400, "text/plain", "Missing parameters");
    }
  });

  server.on("/set-time", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("h", true) && request->hasParam("m", true)) {
      int h = request->getParam("h", true)->value().toInt();
      int m = request->getParam("m", true)->value().toInt();

      clock_manager.set_current_time(h, m, 0);

      request->send_P(200, "text/plain", "OK");
    } else {
      request->send_P(400, "text/plain", "Missing parameters");
    }
  });

  server.on("/set-date", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("d", true) && request->hasParam("m", true) && request->hasParam("y", true)) {
      int d = request->getParam("d", true)->value().toInt();
      int m = request->getParam("m", true)->value().toInt();
      int y = request->getParam("y", true)->value().toInt();

      clock_manager.set_current_date(d, m, y);

      request->send_P(200, "text/plain", "OK");
    } else {
      request->send_P(400, "text/plain", "Missing parameters");
    }
  });

  server.on("/set-tz", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("tz", true)) {
      String tz = request->getParam("tz", true)->value();
      time_zone_manager.set(tz);

      request->send_P(200, "text/plain", "OK");      
    } else {
      request->send_P(400, "text/plain", "Missing parameters");
    }
  });

  server.on("/set-wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
      String ssid = request->getParam("ssid", true)->value();
      String password = request->getParam("password", true)->value();

      wifi_manager.set_credentials(ssid, password);

      request->send(200, "text/plain", ssid);
      delay(3000);
      wifi_manager.begin();

    } else {
      request->send_P(400, "text/plain", "Missing parameters");
    }
  });

  server.on("/calibrate-hour", HTTP_POST, [](AsyncWebServerRequest *request) {
    clock_manager.request_calibrate_hour();
    request->send_P(200, "text/plain", "OK");   
  });

  server.on("/calibrate-minute", HTTP_POST, [](AsyncWebServerRequest *request) {
    clock_manager.request_calibrate_minute();
    request->send_P(200, "text/plain", "OK");   
  });

  server.on("/calibrate-end", HTTP_POST, [](AsyncWebServerRequest *request) {
    clock_manager.request_end_calibrate();
    request->send_P(200, "text/plain", "OK");   
  });

  server.on("/toggle-demo", HTTP_POST, [](AsyncWebServerRequest *request) {
    clock_manager.toggle_demo();
    request->send_P(200, "text/plain", "OK");   
  });

  events.onConnect([](AsyncEventSourceClient *client) {
    if (client->lastId()) {
      Serial.printf("Client reconnected. Last message ID that it got is: %u\n", client->lastId());
    }

    client->send("Connected", NULL, millis(), 500);
  });
  server.addHandler(&events);

  server.begin();
}
#endif

#if ENABLE_BUTTONS == 1
void start_buttons() {
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  left_button.begin();

  left_button.onLongPress([](){
    clock_manager.request_calibrate_hour();
  });

  left_button.onLongRelease([](){
    clock_manager.request_end_calibrate();
  });

  left_button.onClick([]() {
    clock_manager.adjust_displayed_hour(-1);
  });

  left_button.onMultiClick([](int32_t counter){
    if(counter == 2) {
      clock_manager.adjust_displayed_hour(1);
    }
    else if(counter == 3) {
      clock_manager.toggle_demo();
    }
  });

  left_button.enable();

  right_button.begin();

  right_button.onLongPress([](){
    clock_manager.request_calibrate_minute();
  });

  right_button.onLongRelease([](){
    clock_manager.request_end_calibrate();
  });

  right_button.onClick([]() {
    clock_manager.adjust_displayed_minute(-1);
  });

  right_button.onMultiClick([](int32_t counter){
    if(counter == 2) {
      clock_manager.adjust_displayed_minute(1);
    }
    else if(counter == 3) {
      // TO DO: Start setting displayed and current minute
    }
  });

  right_button.enable();
}
#endif
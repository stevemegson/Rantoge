#include <ESPAsyncWebServer.h>
#include <espasyncbutton.hpp>
#include <Preferences.h>
#include <time.h>
#include <sys/time.h>

#include "WifiManager.h"
#include "ClockManager.h"
#include "TimeZoneManager.h"
#include "HTML.h"

AsyncEventButton left_button(GPIO_NUM_16, LOW);
AsyncEventButton right_button(GPIO_NUM_17, LOW);

AsyncWebServer server(80);
AsyncEventSource events("/events");

WifiManager wifi_manager;
ClockManager clock_manager;
TimeZoneManager time_zone_manager;

typedef void (*logger_cb_t) (const char * format, ...);

void setup() {
  Serial.begin(115200);
  delay(1000);

  wifi_manager.begin();
  wifi_manager.start_mdns("clock");

  start_server();
  start_buttons();

  clock_manager.set_logger(send_message);
  clock_manager.start_ntp();
  
  time_zone_manager.set_logger(send_message);
  time_zone_manager.begin();
}

void loop() {
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
  events.send(buffer, "message", millis());
}

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

  events.onConnect([](AsyncEventSourceClient *client) {
    if (client->lastId()) {
      Serial.printf("Client reconnected. Last message ID that it got is: %u\n", client->lastId());
    }

    client->send("Connected", NULL, millis(), 1000);
  });
  server.addHandler(&events);

  server.begin();
}

void start_buttons() {
  left_button.begin();

  left_button.onLongPress([](){
    clock_manager.request_calibrate_hour();
  });

  left_button.onLongRelease([](){
    clock_manager.request_end_calibrate();
  });

  left_button.enable();

  right_button.begin();

  right_button.onLongPress([](){
    clock_manager.request_calibrate_minute();
  });

  right_button.onLongRelease([](){
    clock_manager.request_end_calibrate();
  });

  right_button.enable();
}




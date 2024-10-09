#include "WifiManager.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include "settings.h"

void on_wifi_event(WiFiEvent_t event) {
	switch (event) {
		case WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
			Serial.println("Wi-fi disconnected. Enabling auto-reconnect");
			WiFi.setAutoReconnect(true);
			break;
		default:
      break;
  }
}

void WifiManager::begin() {
  WiFi.removeEvent(on_wifi_event);

  WiFi.disconnect();
  WiFi.setAutoReconnect(false);

  _preferences.begin("wifi", false);
  String ssid = _preferences.getString("ssid", ""); 
  String password = _preferences.getString("password", "");
  String name = _preferences.getString("name", "clock");
  _preferences.end();

  if (ssid != "" && password != ""){
    set_rgb(255, 0, 255);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    Serial.print("Connecting to WiFi ..");
    
    byte retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries++ < 10) {
      Serial.print('.');
      delay(1000);
    }

    if(WiFi.status()== WL_CONNECTED) {
      set_rgb(255,255,0);

      Serial.println("connected");
      Serial.print("   IP address: ");
      Serial.println(WiFi.localIP());

      WiFi.onEvent(on_wifi_event);

      return;
    }
  }

  WiFi.mode(WIFI_AP);
  boolean result = WiFi.softAP(name.c_str(), NULL);
  if (result == true) {
    WiFi.setTxPower(WIFI_POWER_17dBm);

    set_rgb(0, 0, 255);

    Serial.println("AP created");
    Serial.print("   IP address: ");
    Serial.println(WiFi.softAPIP());
  }
  else {
    set_rgb(255,0,0);
    Serial.println("AP failed");
  }
}

void WifiManager::start_mdns() {
  _preferences.begin("wifi", false);
  String name = _preferences.getString("name", "clock");
  _preferences.end();

    if (!MDNS.begin(name.c_str())) {
    Serial.println("Error setting up mDNS responder");
  } else {
    Serial.printf("mDNS responder started: %s.local\n", name);
  }
}

void WifiManager::set_credentials(String ssid, String password) {
  _preferences.begin("wifi", false);
  _preferences.putString("ssid", ssid); 
  _preferences.putString("password", password);
  _preferences.end();

  Serial.print("Stored ssid ");
  Serial.println(ssid);
}

void WifiManager::set_name(String name) {
  _preferences.begin("wifi", false);
  _preferences.putString("name", name); 
  _preferences.end();

  Serial.print("Stored name ");
  Serial.println(name);
}


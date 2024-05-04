#include "WifiManager.h"
#include <WiFi.h>
#include <ESPmDNS.h>

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
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);

  _preferences.begin("wifi", false);
  String ssid = _preferences.getString("ssid", ""); 
  String password = _preferences.getString("password", "");
  _preferences.end();

  if (ssid != "" && password != ""){
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print("Connecting to WiFi ..");
    
    byte retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries++ < 10) {
      Serial.print('.');
      delay(1000);
    }

    if(WiFi.status()== WL_CONNECTED) {
      Serial.println("connected");
      Serial.print("   IP address: ");
      Serial.println(WiFi.localIP());

      WiFi.onEvent(on_wifi_event);

      return;
    }
  }

  WiFi.mode(WIFI_AP);
  boolean result = WiFi.softAP("clock", NULL);
  if (result == true) {
    Serial.println("AP created");
    Serial.print("   IP address: ");
    Serial.println(WiFi.softAPIP());
  }
  else {
    Serial.println("AP failed");
  }
}

void WifiManager::start_mdns(const char* host) {
  if (!MDNS.begin(host)) {
    Serial.println("Error setting up mDNS responder");
  }
  Serial.println("mDNS responder started");
}

void WifiManager::set_credentials(String ssid, String password) {
  _preferences.begin("wifi", false);
  _preferences.putString("ssid", ssid); 
  _preferences.putString("password", password);
  _preferences.end();

  Serial.print("Stored ssid ");
  Serial.println(ssid);
}

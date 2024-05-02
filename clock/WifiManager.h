#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Preferences.h>

class WifiManager {
public:
  void begin();
  void start_mdns(char* chost);
  void set_credentials(String ssid, String password);

private:
  Preferences _preferences;
};

#endif
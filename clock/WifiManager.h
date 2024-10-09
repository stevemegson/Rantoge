#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Preferences.h>

class WifiManager {
public:
  void begin();
  void start_mdns();
  void set_credentials(String ssid, String password);
  void set_name(String name);

private:
  Preferences _preferences;
};

#endif
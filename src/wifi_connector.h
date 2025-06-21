#ifndef WIFI_CONNECTOR_H
#define WIFI_CONNECTOR_H

#include <string>

class WiFiConnector {
public:
    static bool connectToNetwork(const std::string& ssid, const std::string& password);
};

#endif // WIFI_CONNECTOR_H

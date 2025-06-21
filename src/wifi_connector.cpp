#include <windows.h>
#include <string>
#include <sstream>
#include <iostream>
#include <array>
#include <cstdio>

namespace WiFiConnector {

bool connectToNetwork(const std::string& ssid, const std::string& password) {
    // Attempt to connect
    std::string cmd = "netsh wlan connect name=\"" + ssid + "\"";
    system(cmd.c_str());

    // Wait a moment for connection to attempt
    Sleep(3000);

    // Now verify connection
    std::array<char, 128> buffer;
    std::string result;
    FILE* pipe = _popen("netsh wlan show interfaces", "r");
    if (!pipe) return false;

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    _pclose(pipe);

    // Look for connected SSID in output
    std::istringstream stream(result);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.find("State") != std::string::npos && line.find("connected") != std::string::npos) {
            // Confirm the SSID matches
            std::getline(stream, line); // advance to SSID line
            if (line.find("SSID") != std::string::npos && line.find(ssid) != std::string::npos) {
                return true;  // Connected successfully
            }
        }
    }

    return false;  // Not connected
}

}

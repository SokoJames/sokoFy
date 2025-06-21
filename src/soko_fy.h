#ifndef SOKO_FY_H
#define SOKO_FY_H

#include <string>
#include <vector>
#include <utility>

enum class SecurityType {
    OPEN,
    SECURED,
    UNKNOWN
};

class SokoFy {
public:
    // Updated constructor to include SSID as the first parameter
    SokoFy(const std::string& ssid, const std::string& client_mac, const std::string& ap_mac, const std::string& psk, SecurityType sec_type);

    void startHandshake();

    SecurityType getSecurityType() const { return security_type_; }

private:
    std::string ssid_;         // Added SSID member
    std::string client_mac_;
    std::string ap_mac_;
    std::string psk_;
    SecurityType security_type_;

    std::string anonce_;
    std::string snonce_;
    std::string pmk_;
    std::string ptk_;

    void generateAnonce();
    void generateSnonce();
    std::string derivePMK(const std::string& psk);
    std::string derivePTK();
    std::string generateNonce();
    void sendMessage(int msg_number);
};

void scanWiFiNetworks(std::vector<std::pair<std::string, SecurityType>>& ssidMacSecList);
void showOwnMacAddress();
bool isValidMac(const std::string& mac);
std::string normalizeMac(const std::string& mac);
bool isValidPSK(const std::string& psk);

#endif // SOKO_FY_H

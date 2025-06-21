#ifndef HANDSHAKE_H
#define HANDSHAKE_H

#include <string>

class Handshake {
private:
    std::string ssid;
    std::string password;
    std::string clientMac;
    std::string apMac;

    std::string anonce;
    std::string snonce;
    std::string pmk;
    std::string ptk;

    bool isValidMac(const std::string& mac);

    std::string generateNonce();
    std::string derivePMK(const std::string& password, const std::string& ssid);
    std::string derivePTK(const std::string& pmk, const std::string& anonce, const std::string& snonce);

public:
    Handshake(const std::string& ssid, const std::string& password,
              const std::string& clientMac, const std::string& apMac);

    bool performHandshake();
};

#endif

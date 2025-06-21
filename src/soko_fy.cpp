#include "soko_fy.h"
#include "wifi.h"
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <regex>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

// Set colors
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define YELLOW  "\033[33m"
#define WHITE   "\033[37m"
#define RED     "\033[31m"
#define ORANGE "\033[38;5;208m"
#define BOLD    "\033[1m"
#define CLEAR   "\033[2J\033[H"  // Clears screen and moves cursor to top-left
#define RESET   "\033[0m"

SokoFy::SokoFy(const std::string& ssid, const std::string& client_mac, const std::string& ap_mac, const std::string& psk, SecurityType sec_type)
    : ssid_(ssid), client_mac_(client_mac), ap_mac_(ap_mac), psk_(psk), security_type_(sec_type) {
    if (security_type_ == SecurityType::SECURED)
        pmk_ = derivePMK(psk_);
}

void SokoFy::startHandshake() {
    if (security_type_ == SecurityType::OPEN) {
        std::cout << std::endl;
        std::cout << GREEN "[Open Wi-Fi]" << RESET << " No handshake needed. Connecting...\n";

        // Use netsh to connect to the open Wi-Fi network by SSID
        std::string cmd = "netsh wlan connect name=\"" + ssid_ + "\"";
        int result = system(cmd.c_str());

        if (result == 0)
            std::cout << GREEN "[-]" << RESET << " Connected to open Wi-Fi successfully!\n";
        else
            std::cout << RED "[x]" << RESET << " Failed to connect to open Wi-Fi.\n";

        return;
    }
    std::cout << std::endl;
    std::cout << YELLOW << "                                   4-WAY HANDSHAKE" << RESET << std::endl;
    std::cout << "_________________________________________________________________________________________" << std::endl;
    std::cout << std::endl;

    generateAnonce();
    sendMessage(1);

    generateSnonce();
    sendMessage(2);

    ptk_ = derivePTK();
    sendMessage(3);
    sendMessage(4);
    std::cout << "_________________________________________________________________________________________" << std::endl;
}

void SokoFy::generateAnonce() {
    anonce_ = generateNonce();
    std::cout << std::left << std::setw(16) << "AP"
              << "Generated ANonce " << anonce_ << "\n";
}

void SokoFy::generateSnonce() {
    snonce_ = generateNonce();
    std::cout << std::left << std::setw(16) << "Client"
              << "Generated SNonce " << snonce_ << "\n";
}

std::string SokoFy::derivePMK(const std::string& psk) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(psk.c_str()), psk.length(), hash);

    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    return oss.str();
}

std::string SokoFy::derivePTK() {
    std::string data = anonce_ + snonce_ + ap_mac_ + client_mac_;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), hash);

    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    std::cout << std::left << std::setw(16) << "Derived PTK"
              << "Results " << oss.str() << "\n";

    return oss.str();
}

std::string SokoFy::generateNonce() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    std::ostringstream oss;
    for (int i = 0; i < 16; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << dis(gen);

    return oss.str();
}

void SokoFy::sendMessage(int msg_number) {
    switch (msg_number) {
        case 1:
            std::cout << std::left << std::setw(16) << "Message 1"
                      << "AP sends ANonce to Client\n";
            break;
        case 2:
            std::cout << std::left << std::setw(16) << "Message 2"
                      << "Client sends SNonce and MIC to AP\n";
            break;
        case 3:
            std::cout << std::left << std::setw(16) << "Message 3"
                      << "AP sends install PTK message to Client\n";
            break;
        case 4:
            std::cout << std::left << std::setw(16) << "Message 4"
                      << "Client confirms install\n";
            break;
        default:
            std::cerr << RED << "[x]" << RESET << " Invalid message number\n";
    }
}

void scanWiFiNetworks(std::vector<std::pair<std::string, SecurityType>>& ssidMacSecList) {
    std::cout << std::endl;
    std::cout << BLUE;
    printAsciiLogo();
    std::cout << RESET;
    std::cout << std::endl;
    std::cout << YELLOW << "AVAILABLE WI-FI NETWORKS" << RESET << std::endl;
    std::cout << std::endl;

    const char* tempFile = "wifi_scan_output.txt";
    std::string cmd = "netsh wlan show networks mode=bssid > " + std::string(tempFile);
    system(cmd.c_str());

    std::ifstream infile(tempFile);
    std::string line;
    std::string currentSSID;
    std::string currentSecurity;
    std::string currentBSSID;

    ssidMacSecList.clear();

    while (std::getline(infile, line)) {
        if (line.find("SSID ") != std::string::npos && line.find(" : ") != std::string::npos) {
            size_t pos = line.find(" : ");
            currentSSID = line.substr(pos + 3);
        }

        if (line.find("BSSID ") != std::string::npos && line.find(" : ") != std::string::npos) {
            size_t pos = line.find(" : ");
            currentBSSID = line.substr(pos + 3);
        }

        if (line.find("Authentication") != std::string::npos && line.find(" : ") != std::string::npos) {
            size_t pos = line.find(" : ");
            currentSecurity = line.substr(pos + 3);

            SecurityType secType = SecurityType::UNKNOWN;
            if (currentSecurity.find("Open") != std::string::npos)
                secType = SecurityType::OPEN;
            else if (currentSecurity.find("WPA") != std::string::npos || currentSecurity.find("WEP") != std::string::npos)
                secType = SecurityType::SECURED;

            if (!currentSSID.empty() && !currentBSSID.empty()) {
                ssidMacSecList.emplace_back(currentSSID + " (" + currentBSSID + ")", secType);
            }

            currentBSSID.clear();
            currentSecurity.clear();
        }
    }
    infile.close();
    std::remove(tempFile);

    int idx = 1;
    for (auto& net : ssidMacSecList) {
        std::cout << idx++ << ". " << net.first << " [" 
                  << (net.second == SecurityType::OPEN ? "Open" : 
                      net.second == SecurityType::SECURED ? "Secured" : "Unknown") << "]\n";
    }
    std::cout << std::endl;
}

void showOwnMacAddress() {
    std::cout << YELLOW << "YOUR NETWORK ADAPTERS (MAC ADDRESSES)" << RESET << std::endl;
    system("ipconfig /all");
}

bool isValidMac(const std::string& mac) {
    std::regex colon_hyphen_regex("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$");
    std::regex dot_regex("^([0-9A-Fa-f]{4}\\.){2}([0-9A-Fa-f]{4})$");

    return std::regex_match(mac, colon_hyphen_regex) || std::regex_match(mac, dot_regex);
}

std::string normalizeMac(const std::string& mac) {
    std::string normalized;

    if (mac.find('.') != std::string::npos) {
        if (mac.length() != 14)
            return "";
        std::string hexOnly;
        for (char c : mac) {
            if (c != '.')
                hexOnly.push_back(toupper(c));
        }
        if (hexOnly.length() != 12)
            return "";

        for (size_t i = 0; i < 12; i += 2) {
            normalized += hexOnly.substr(i, 2);
            if (i < 10)
                normalized += ":";
        }
        return normalized;
    }

    // For colon or hyphen separated formats
    if (mac.length() != 17)
        return "";

    for (size_t i = 0; i < mac.length(); ++i) {
        char c = mac[i];
        if ((i + 1) % 3 == 0) {
            if (c != ':' && c != '-')
                return "";
            normalized += ':';
        } else {
            if (!isxdigit(c))
                return "";
            normalized += toupper(c);
        }
    }
    return normalized;
}

bool isValidPSK(const std::string& psk) {
    return (psk.length() >= 8 && psk.length() <= 63);
}

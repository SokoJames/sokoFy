#include "Handshake.h"
#include <iostream>
#include <regex>
#include <random>

// Set colors
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define YELLOW  "\033[33m"
#define WHITE   "\033[37m"
#define RED     "\033[31m"
#define ORANGE  "\033[38;5;208m"
#define BOLD    "\033[1m"
#define CLEAR   "\033[2J\033[H"  // Clears screen and moves cursor to top-left
#define RESET   "\033[0m"

Handshake::Handshake(const std::string& ssid, const std::string& password,
                     const std::string& clientMac, const std::string& apMac)
    : ssid(ssid), password(password), clientMac(clientMac), apMac(apMac) {}

bool Handshake::isValidMac(const std::string& mac) {
    std::regex macRegex("^([0-9A-Fa-f]{2}:){5}([0-9A-Fa-f]{2})$");
    return std::regex_match(mac, macRegex);
}

std::string Handshake::generateNonce() {
    const char hexChars[] = "0123456789ABCDEF";
    std::string nonce;
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < 32; ++i) {
        nonce += hexChars[gen() % 16];
    }
    return nonce;
}

std::string Handshake::derivePMK(const std::string& password, const std::string& ssid) {
    return "PMK_" + password + "_" + ssid;
}

std::string Handshake::derivePTK(const std::string& pmk, const std::string& anonce, const std::string& snonce) {
    return "PTK_" + pmk + "_" + anonce + "_" + snonce;
}

bool Handshake::performHandshake() {
    if (!isValidMac(clientMac) || !isValidMac(apMac)) {
        std::cerr << RED << "[x]" << RESET << " Invalid MAC address format.\n";
        return false;
    }

    std::cout << std::endl;
    std::cout << YELLOW << "                                        4-WAY HANDSHAKE SIMULATION" << RESET << std::endl;
    std::cout << std::endl;
    std::cout << RED << "SSID" << RESET << "                          " << ssid << "\n";
    std::cout << RED << "Client MAC" << RESET << "                    " << clientMac << "\n";
    std::cout << RED << "AP MAC" << RESET << "                        " << apMac << "\n";

    // Step 1: AP sends ANonce to Client
    anonce = generateNonce();
    std::cout << RED << "Step 1 AP sends ANonce" << RESET << "        " << anonce << "\n";

    // Step 2: Client sends SNonce + MIC
    snonce = generateNonce();
    std::cout << RED << "Step 2 Client sends SNonce" << RESET << "    " << snonce << "\n";

    // Step 3: PMK & PTK derivation
    pmk = derivePMK(password, ssid);
    ptk = derivePTK(pmk, anonce, snonce);

    std::cout << RED << "Derived PMK" << RESET << "                   " << pmk << "\n";
    std::cout << RED << "Derived PTK" << RESET << "                   " << ptk << "\n";

    // Step 4: AP validates PTK (simulated here by recalculating and comparing)
    std::string apPTK = derivePTK(pmk, anonce, snonce);

    std::cout << RED << "AP derived PTK" << RESET << "                " << apPTK << "\n";

    if (ptk == apPTK) {
    std::cout << RED << "Handshake Success" << RESET << "            " << GREEN << " PTK Match!" << RESET << std::endl;
        return true;
    } else {
    std::cout << RED << "Handshake Failed" << RESET << "             " << RED << " PTK Mismatch!" << RESET << std::endl;
        return false;
    }
}

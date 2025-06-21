#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <limits>
#include "soko_fy.h"
#include "soko_fy_logo.h"
#include "wifi_connector.h"
#include "Handshake.h"
#include "HandshakeVisualizer.h"

#include <windows.h>

void enableVirtualTerminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

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

bool askReturnOrExit() {
    while (true) {
        std::cout << ORANGE << "[?]" << RESET << " Do you want to return to the main menu? (" << GREEN << "y" << RESET << "/" << RED << "n" << RESET << "): ";
        std::string input;
        std::getline(std::cin, input);
        if (input == "Y" || input == "y") return true;
        if (input == "N" || input == "n") return false;
        std::cout << RED << "[x]" << RESET << " Invalid choice. Please enter 'y' or 'n'.\n";
    }
}

int main() {
    enableVirtualTerminal();

    bool runProgram = true;

    while (runProgram) {
        std::cout << CLEAR; // clear screen each time menu shows up
        std::cout << RED;
        printSokoFyLogo();
        std::cout << RESET;

        int choiceMain;
        std::cout << YELLOW << "[1]" << RESET << " Simulated 4-way handshake (Hidden Network)" << std::endl;
        std::cout << YELLOW << "[2]" << RESET << " Simulated 4-way handshake" << std::endl;
        std::cout << YELLOW << "[3]" << RESET << " Actual 4-way handshake" << std::endl;
        std::cout << YELLOW << "[4]" << RESET << " 4-way handshake visualizer" << std::endl;
        std::cout << YELLOW << "[0]" << RESET << " Exit" << std::endl;
        std::cout << std::endl;
        std::cout << YELLOW << " > " << RESET;

        if (!(std::cin >> choiceMain)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << RED << "[x]" << RESET << " Invalid input. Please enter a number.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear newline
        std::cout << std::endl;

        if (choiceMain == 0) {
            std::cout << RED << "Exiting..." << RESET << YELLOW << " O_O" << RESET << " Thank you for using sokoFy." << std::endl << std::endl;
            runProgram = false;
            continue;
        }

        if (choiceMain == 2) {
            // Your existing simulation 4-way handshake code block here
            showOwnMacAddress();

            std::vector<std::pair<std::string, SecurityType>> networks;
            scanWiFiNetworks(networks);

            if (networks.empty()) {
                std::cout << RED << "[x]" << RESET << " No Wi-Fi networks detected.\n";
                if (!askReturnOrExit()) break;
                else continue;
            }

            int choiceNetwork = 0;
            while (true) {
                std::cout << YELLOW << "[>]" << RESET << " Select the Wi-Fi network to connect to (number): ";
                std::string input;
                std::getline(std::cin, input);
                try {
                    choiceNetwork = std::stoi(input);
                    if (choiceNetwork >= 1 && choiceNetwork <= (int)networks.size()) {
                        break;
                    }
                } catch (...) {}
                std::cout << RED << "[x]" << RESET << " Invalid choice. Try again.\n";
            }

            std::string ssid_mac = networks[choiceNetwork - 1].first;

            size_t start = ssid_mac.find('(');
            size_t end = ssid_mac.find(')');
            if (start == std::string::npos || end == std::string::npos || end <= start) {
                std::cout << RED << "[x]" << RESET << " Failed to parse AP MAC address.\n";
                if (!askReturnOrExit()) break;
                else continue;
            }
            std::string ssid = ssid_mac.substr(0, start - 1);
            std::string ap_mac_raw = ssid_mac.substr(start + 1, end - start - 1);
            std::string ap_mac = normalizeMac(ap_mac_raw);

            if (ap_mac.empty()) {
                std::cout << RED << "[x]" << RESET << " Invalid AP MAC address format detected: " << ap_mac_raw << "\n";
                if (!askReturnOrExit()) break;
                else continue;
            }

            std::cout << YELLOW << "[>]" << RESET << " Selected AP MAC: " << ap_mac << "\n";

            std::string client_mac;
            while (true) {
                std::cout << YELLOW << "[>]" << RESET << " Enter your Client MAC address (format XX:XX:XX:XX:XX:XX): ";
                std::getline(std::cin, client_mac);
                client_mac = normalizeMac(client_mac);
                if (!client_mac.empty()) break;
                std::cout << RED << "[x]" << RESET << " Invalid client MAC address format. Try again.\n";
            }

            std::string psk;

            if (networks[choiceNetwork - 1].second == SecurityType::SECURED) {
                while (true) {
                    std::cout << YELLOW << "[>]" << RESET << " Enter Wi-Fi key (8-63 chars): ";
                    std::getline(std::cin, psk);
                    if (isValidPSK(psk)) break;
                    std::cout << RED << "[x]" << RESET << " Invalid Wi-Fi key length. Try again.\n";
                }
            } else {
                std::cout << std::endl;
                std::cout << GREEN << "[-]" << RESET << " Open Wi-Fi detected, no key required.\n";
            }

            SokoFy soko(ssid, client_mac, ap_mac, psk, networks[choiceNetwork - 1].second);
            soko.startHandshake();

            std::cout << std::endl;
            std::cout << YELLOW << "O_O" << RESET << " Thank you for using sokoFy." << std::endl;
            std::cout << std::endl;

            if (!askReturnOrExit()) break;
        }
        else if (choiceMain == 3) {
            // Your existing actual 4-way handshake code block here
            showOwnMacAddress();

            std::vector<std::pair<std::string, SecurityType>> networks;
            scanWiFiNetworks(networks);

            if (networks.empty()) {
                std::cout << RED << "[x]" << RESET << " No Wi-Fi networks detected.\n";
                if (!askReturnOrExit()) break;
                else continue;
            }

            int selected = 0;
            while (true) {
                std::cout << YELLOW << "[>] " << RESET << "Select the Wi-Fi network to connect to (number): ";
                std::string input;
                std::getline(std::cin, input);
                try {
                    selected = std::stoi(input);
                    if (selected >= 1 && selected <= (int)networks.size()) {
                        break;
                    }
                } catch (...) {}
                std::cout << RED << "[x]" << RESET << " Invalid choice. Try again.\n";
            }

            std::string ssid_mac = networks[selected - 1].first;
            size_t start = ssid_mac.find('(');
            size_t end = ssid_mac.find(')');
            std::string ssid = ssid_mac.substr(0, start - 1);
            std::string ap_mac_raw = ssid_mac.substr(start + 1, end - start - 1);
            std::string ap_mac = normalizeMac(ap_mac_raw);

            if (ap_mac.empty()) {
                std::cout << RED << "[x]" << RESET << " Invalid AP MAC address format.\n";
                if (!askReturnOrExit()) break;
                else continue;
            }

            std::cout << YELLOW << "[>] " << RESET << "Selected AP MAC: " << ap_mac << "\n";

            std::string client_mac;
            while (true) {
                std::cout << YELLOW << "[>] " << RESET << "Enter your Client MAC address (format XX:XX:XX:XX:XX:XX): ";
                std::getline(std::cin, client_mac);
                client_mac = normalizeMac(client_mac);
                if (!client_mac.empty()) break;
                std::cout << RED << "[x]" << RESET << " Invalid client MAC address format. Try again.\n";
            }

            std::string psk;
            if (networks[selected - 1].second == SecurityType::SECURED) {
                while (true) {
                    std::cout << YELLOW << "[>] " << RESET << "Enter Wi-Fi key (8-63 chars): ";
                    std::getline(std::cin, psk);
                    if (isValidPSK(psk)) break;
                    std::cout << RED << "[x]" << RESET << " Invalid Wi-Fi key length. Try again.\n";
                }

                bool connected = WiFiConnector::connectToNetwork(ssid, psk);
                if (connected) {
                    std::cout << GREEN << "[-]" << RESET << " Connected successfully to " << ssid << "!\n";
                } else {
                    std::cout << RED << "[x]" << RESET << " Failed to connect to " << ssid << ".\n\n";

                    // Simulate 4-way handshake after failed connection
                    SokoFy soko(ssid, client_mac, ap_mac, psk, SecurityType::SECURED);
                    soko.startHandshake();
                }
            } else {
                // Open Wi-Fi
                bool connected = WiFiConnector::connectToNetwork(ssid, "");
                if (connected) {
                    std::cout << std::endl;
                    std::cout << GREEN << "[-]" << RESET << " Connected successfully to " << ssid << "!\n";
                } else {
                    std::cout << std::endl;
                    std::cout << RED << "[x]" << RESET << " Failed to connect to " << ssid << ".\n";
                }
            }

            std::cout << std::endl;
            std::cout << YELLOW << "O_O" << RESET << " Thank you for using sokoFy." << std::endl;
            std::cout << std::endl;

            if (!askReturnOrExit()) break;
        }

        else if (choiceMain == 1){
            std::string ssid, password, clientMac, apMac;

            std::cout << YELLOW << "[>]" << RESET << " Enter SSID: ";
            std::getline(std::cin, ssid);

            std::cout << YELLOW << "[>]" << RESET << " Enter Password: ";
            std::getline(std::cin, password);

            std::cout << YELLOW << "[>]" << RESET << " Enter Client MAC Address (format XX:XX:XX:XX:XX:XX): ";
            std::getline(std::cin, clientMac);

            std::cout << YELLOW << "[>]" << RESET << " Enter AP MAC Address (format XX:XX:XX:XX:XX:XX): ";
            std::getline(std::cin, apMac);

            Handshake hs(ssid, password, clientMac, apMac);
            hs.performHandshake();

            std::cout << std::endl;
            std::cout << YELLOW << "O_O" << RESET << " Thank you for using sokoFy." << std::endl;
            std::cout << std::endl;

            if (!askReturnOrExit()) break;
        }

        else if (choiceMain == 4){
            try {
                    HandshakeVisualizer visualizer(3); // 3 seconds delay between steps
                    visualizer.run();
                } catch (const std::exception& e) {
                        std::cerr << "Error: " << e.what() << "\n";
                        return 1;
                }

                std::cout << std::endl;
                std::cout << YELLOW << "O_O" << RESET << " Thank you for using sokoFy." << std::endl;
                std::cout << std::endl;

                if (!askReturnOrExit()) break;
        }

        else {
            std::cout << RED << "[x]" << RESET << " Invalid menu option.\n";
            // Just continue loop
        }
    }

    return 0;
}

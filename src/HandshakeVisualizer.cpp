#include "HandshakeVisualizer.h"
#include <iostream>
#include <thread>
#include <chrono>

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

HandshakeVisualizer::HandshakeVisualizer(int delaySeconds) : delay(delaySeconds) {}

void HandshakeVisualizer::wait() {
    std::this_thread::sleep_for(std::chrono::seconds(delay));
}

void HandshakeVisualizer::printSeparator() {
}

void HandshakeVisualizer::printTitle() {
    std::cout << "\n"
              << YELLOW << "                     WPA2 4-WAY HANDSHAKE SIMULATION" << RESET << std::endl;
              std::cout << std::endl;
}

void HandshakeVisualizer::printDevices() {
    std::cout << BLUE;
    std::cout <<
        "    +------------------+                         +--------------------+\n"
        "    |   CLIENT (STA)   |                         |  ACCESS POINT (AP) |\n"
        "    +------------------+                         +--------------------+\n\n";
        std::cout << RESET;
}

void HandshakeVisualizer::printStep(int step) {
    switch (step) {
        case 1:
            std::cout <<
                "Step 1: AP sends ANonce (Authenticator Nonce) to Client\n\n"
                "    +------------------+                         +--------------------+\n"
                "    |                  |                         |                    |\n"
                "    |                  | <-----------------------|   ANonce (Random)  |\n"
                "    |                  |                         |                    |\n"
                "    +------------------+                         +--------------------+\n\n";
            break;

        case 2:
            std::cout <<
                "Step 2: Client sends SNonce (Supplicant Nonce) + MIC (Message Integrity Code) to AP\n\n"
                "    +------------------+                         +--------------------+\n"
                "    |                  |                         |                    |\n"
                "    |   SNonce + MIC   | ----------------------> |                    |\n"
                "    |                  |                         |                    |\n"
                "    +------------------+                         +--------------------+\n\n";
            break;

        case 3:
            std::cout <<
                "Step 3: AP sends Group Temporal Key (GTK) + MIC to Client\n\n"
                "    +------------------+                         +--------------------+\n"
                "    |                  |                         |                    |\n"
                "    |                  | <-----------------------|     GTK + MIC      |\n"
                "    |                  |                         |                    |\n"
                "    +------------------+                         +--------------------+\n\n";
            break;

        case 4:
            std::cout <<
                " Step 4: Client sends ACK (Acknowledgement) to AP\n\n"
                "    +------------------+                         +--------------------+\n"
                "    |                  |                         |                    |\n"
                "    |       ACK        | ----------------------> |                    |\n"
                "    |                  |                         |                    |\n"
                "    +------------------+                         +--------------------+\n\n";
            break;
    }
}

void HandshakeVisualizer::run() {
    printTitle();
    printDevices();

    for (int step = 1; step <= 4; ++step) {
        printStep(step);
        wait();
        printSeparator();
    }

    std::cout << GREEN  "\n            Handshake Complete! Secure connection established" << RESET << std::endl;
    std::cout << std::endl;
}

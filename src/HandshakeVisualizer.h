#ifndef HANDSHAKEVISUALIZER_H
#define HANDSHAKEVISUALIZER_H

class HandshakeVisualizer {
public:
    HandshakeVisualizer(int delaySeconds = 3);
    void run();

private:
    int delay; // delay in seconds between steps

    void printSeparator();
    void printTitle();
    void printDevices();
    void printStep(int step);
    void wait();
};

#endif // HANDSHAKEVISUALIZER_H

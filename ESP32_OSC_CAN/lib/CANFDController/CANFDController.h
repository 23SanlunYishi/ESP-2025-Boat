#ifndef CANFDCONTROLLER_H
#define CANFDCONTROLLER_H

#include <SPI.h>
#include <ACAN2517FD.h>

class CANFDController
{
public:
    CANFDController(byte csPin = 9, byte intPin = 6);

    void setup();
    void send(CANFDMessage frame);
    void receive();

    static void isr(); // 割り込みサービスルーチンを静的関数として宣言

private:
    ACAN2517FD can;
    byte csPin;
    byte intPin;
    static uint32_t gReceivedFrameCount;
    static uint32_t gSentFrameCount;
    static CANFDController *instance;
};

#endif // CANFDCONTROLLER_H

#include "CANFDController.h"

// 静的メンバの定義
uint32_t CANFDController::gReceivedFrameCount = 0;
uint32_t CANFDController::gSentFrameCount = 0;
CANFDController *CANFDController::instance = nullptr;

CANFDController::CANFDController(byte csPin, byte intPin)
    : can(csPin, SPI, intPin), csPin(csPin), intPin(intPin)
{
    instance = this; // 現在のインスタンスを静的メンバに格納
}

void CANFDController::setup()
{
    // INT1 INT2使わない
    pinMode(4, INPUT);
    pinMode(5, INPUT);
    //--- Start serial
    Serial.begin(115200);
    //--- Wait for serial (blink led at 10 Hz during waiting)
    while (!Serial)
    {
        delay(50);
    }
    //----------------------------------- Begin SPI
    SPI.begin();
    //--- Configure ACAN2517FD
    Serial.print("sizeof (ACAN2517FDSettings): ");
    Serial.print(sizeof(ACAN2517FDSettings));
    Serial.println(" bytes");
    Serial.println("Configure ACAN2517FD");
    //--- For version >= 2.1.0
    ACAN2517FDSettings settings(ACAN2517FDSettings::OSC_40MHz, 2 * 1000 * 1000, DataBitRateFactor::x1);
    //--- For version < 2.1.0
    //  ACAN2517FDSettings settings (ACAN2517FDSettings::OSC_4MHz10xPLL, 125 * 1000, ACAN2517FDSettings::DATA_BITRATE_x1) ;
    settings.mRequestedMode = ACAN2517FDSettings::NormalFD; // Select loopback mode
    //--- RAM Usage
    Serial.print("MCP2517FD RAM Usage: ");
    Serial.print(settings.ramUsage());
    Serial.println(" bytes");
    //--- Begin
    const uint32_t errorCode = can.begin(settings, isr); // 静的関数isrを渡す
    if (errorCode == 0)
    {
        Serial.print("Bit Rate prescaler: ");
        Serial.println(settings.mBitRatePrescaler);
        Serial.print("Arbitration Phase segment 1: ");
        Serial.println(settings.mArbitrationPhaseSegment1);
        Serial.print("Arbitration Phase segment 2: ");
        Serial.println(settings.mArbitrationPhaseSegment2);
        Serial.print("Arbitration SJW:");
        Serial.println(settings.mArbitrationSJW);
        Serial.print("Actual Arbitration Bit Rate: ");
        Serial.print(settings.actualArbitrationBitRate());
        Serial.println(" bit/s");
        Serial.print("Exact Arbitration Bit Rate ? ");
        Serial.println(settings.exactArbitrationBitRate() ? "yes" : "no");
        Serial.print("Arbitration Sample point: ");
        Serial.print(settings.arbitrationSamplePointFromBitStart());
        Serial.println("%");
    }
    else
    {
        Serial.print("Configuration error 0x");
        Serial.println(errorCode, HEX);
    }
}

void CANFDController::send(CANFDMessage frame)
{
    frame.id = 0;
    frame.len = 64;

    const bool ok = can.tryToSend(frame);
    if (ok)
    {
        gSentFrameCount += 1;
        Serial.print("Sent: ");
        Serial.println(gSentFrameCount);
    }
    else
    {
        Serial.println("Send failure");
    }
}

void CANFDController::receive()
{
    if (can.available())
    {
        CANFDMessage frame;
        can.receive(frame);
        gReceivedFrameCount++;
        Serial.print("Received: ");
        Serial.print(frame.len);
        Serial.print(", ");
        Serial.print(frame.dataFloat[0]);
        Serial.print(", ");
        Serial.print(frame.dataFloat[1]);
        Serial.print(", ");
        Serial.println(frame.dataFloat[2]);
    }
}

void CANFDController::isr()
{
    if (instance != nullptr)
    {
        instance->can.isr();
    }
}

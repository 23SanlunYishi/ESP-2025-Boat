#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOSCWiFi.h>
#include <SPI.h>
const char* ssid     = "D2-Score-2.4GHz";
const char* password = "robomech-d2";

const int receivePort = 8000;  // Unity が送ってくるやつ

void setup()
{
  Serial.begin(115200);

  // WiFi 接続
  WiFi.begin(ssid, password);
  Serial.println("Connecting WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // ===== Unity → ESP32 =====
  // /pose xRot zRot yPos 受信
  OscWiFi.subscribe(receivePort, "/pose", [](OscMessage& m)
  {
    float xRot = m.arg<float>(0);
    float zRot = m.arg<float>(1);
    float yPos = m.arg<float>(2);

    Serial.print("Recv /pose : ");
    Serial.print("xRot="); Serial.print(xRot);
    Serial.print(", zRot="); Serial.print(zRot);
    Serial.print(", yPos="); Serial.println(yPos);
  });
}

void loop()
{
  OscWiFi.update();
}

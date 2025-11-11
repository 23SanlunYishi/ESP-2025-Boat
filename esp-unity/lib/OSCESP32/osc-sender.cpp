#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOSCWiFi.h>
// ===== Wi-Fi設定 =====
const char* ssid     = "Buffalo-G-23D6";  //（または同一LANのSSID）
const char* password = "FuEjan9m5Sp4";         // そのパスワード
const char* host     = "192.168.11.2";          // ★MacのIPに必ず変更（例）
const int   port     = 8000;                   // Unity側OSCポート


const unsigned long interval = 1000;
unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() - lastSendTime > interval) {
    lastSendTime = millis();
    // /OscJack/sample に (string, float, int) を送信
    OscWiFi.send(host, port, "/OscJack/sample", "HelloUnity", 0.5f, 42);
    Serial.println("Sent OSC message: /OscJack/sample HelloUnity 0.5 42");
  }
  // 送信だけならなくても動くが、将来受信もするなら置いておくと良い
  OscWiFi.update();
}

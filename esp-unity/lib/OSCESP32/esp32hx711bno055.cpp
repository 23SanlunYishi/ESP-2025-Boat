#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOSCWiFi.h>
#include <Adafruit_BNO055.h>
#include <HX711.h>

// ===== WiFi =====
const char* ssid     = "Buffalo-G-23D6";
const char* password = "FuEjan9m5Sp4";
const char* host     = "192.168.11.2";   // ★ Unityが動くMacのIP
const int   port     = 9000;             // ★ Unityが受信するポート（Unity側の receivePort ）

// ===== BNO055 =====
Adafruit_BNO055 bno = Adafruit_BNO055(55);

// ===== HX711（左右ロードセル） =====
HX711 scaleLeft;
HX711 scaleRight;

const int HX_L_DT = 18;
const int HX_L_SCK = 19;

const int HX_R_DT = 16;
const int HX_R_SCK = 17;

unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  // I2C 初期化（明示的に SDA=21, SCL=22 を指定）
  Wire.begin(21, 22);
  // ---- WiFi ----
  WiFi.begin(ssid, password);
  Serial.println("Connecting WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300); Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());

  // ---- BNO055 ----
  if (!bno.begin()) {
    Serial.println("BNO055 NOT FOUND!");
    while (1);
  }
  delay(1000);

  // ---- HX711 ----
  scaleLeft.begin(HX_L_DT, HX_L_SCK);
  scaleRight.begin(HX_R_DT, HX_R_SCK);
  
  scaleLeft.set_scale(1.0);
  scaleRight.set_scale(1.0);
}

void loop() {
  if (millis() - lastSend > 20) {  // 50Hz
    lastSend = millis();

    // ---- BNO055 ----
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    float roll  = euler.x();
    float pitch = euler.y();
    float yaw   = euler.z();

    // ---- Load cell ----
    float loadL = scaleLeft.get_units(1);
    float loadR = scaleRight.get_units(1);

    // ---- OSC送信 (/sensor) ----
    OscWiFi.send(host, port, "/sensor", roll, pitch, yaw, loadL, loadR);

    Serial.printf("Send /sensor R=%.2f P=%.2f Y=%.2f L=%.2f R=%.2f\n",
                  roll, pitch, yaw, loadL, loadR);
  }

  OscWiFi.update();
}

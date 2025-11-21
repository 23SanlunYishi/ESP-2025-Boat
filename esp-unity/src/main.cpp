#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOSCWiFi.h>
#include <Adafruit_BNO055.h>
#include <HX711.h>

// ===== WiFi =====
const char* ssid     = "VR-Boat-2.4GHz";//D2-Score-2.4GHz
const char* password = "robomech-vr";   //robomech-d2
// const char* ssid = "kaji-lab-Guest";//梶本研ゲストWiFi
// const char* password = "n8i!7fN9Cu62SZV";//梶本研ゲストWiFiパスワード
//const char* ssid     = "Buffalo-5510";//自宅のWiFiSSID
//const char* password = "syw7vc8srgngf";   //自宅のWiFiパスワード
//const char* host     = "192.168.11.3";   // ★ Unityが動くMacのIP
const char* host     = "192.168.10.120";   // ★ Unity固定
//const char* host     = "172.16.80.233";   // ★ Unityが動くQuestのIP（自宅）
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
    // 方向は一括送信、左右荷重は明示的に別アドレスで送る（Unityでミスを減らす）
    OscWiFi.send(host, port, "/sensor/orientation", roll, pitch, yaw);
    OscWiFi.send(host, port, "/sensor/left", loadL);
    OscWiFi.send(host, port, "/sensor/right", loadR);

    // ログ：ラベルを明確に
    Serial.printf("Send /sensor orientation R=%.2f P=%.2f Y=%.2f  Left=%.2fg  Right=%.2fg\n",
                  roll, pitch, yaw, loadL, loadR);
  }

  OscWiFi.update();
}

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOSCWiFi.h>
#include <Adafruit_BNO055.h>
#include <HX711.h>

// ===== WiFi =====
const char* ssid     = "Buffalo-G-23D6";
const char* password = "FuEjan9m5Sp4";
const char* host     = "192.168.11.2";
const int   port     = 9000;

// ===== BNO055 =====
Adafruit_BNO055 bno = Adafruit_BNO055(55);

// ===== HX711 =====
HX711 scaleLeft;
HX711 scaleRight;

const int HX_L_DT = 18;
const int HX_L_SCK = 19;
const int HX_R_DT = 16;
const int HX_R_SCK = 17;

unsigned long lastSend = 0;

// 生値をちょっとだけ平均して読む関数
long readRaw(HX711& scale, int samples = 5) {
  long sum = 0;
  for (int i = 0; i < samples; ++i) {
    sum += scale.read();
  }
  return sum / samples;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  // WiFi.begin(ssid, password);
  // Serial.println("Connecting WiFi...");
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(300); Serial.print(".");
  // }
  // Serial.println("\nWiFi connected!");
  // Serial.println(WiFi.localIP());

  if (!bno.begin()) {
    Serial.println("BNO055 NOT FOUND!");
    while (1);
  }
  delay(1000);

  scaleLeft.begin(HX_L_DT, HX_L_SCK);
  scaleRight.begin(HX_R_DT, HX_R_SCK);

  // 符号だけそろえたいならここで片方を反転
  // （まずは両方そのままでもOK。Unity側で差分を見るので）
  // scaleLeft.set_scale(-1.0f);
  // scaleRight.set_scale(1.0f);
}

void loop() {
  if (millis() - lastSend > 20) {  // 50Hz
    lastSend = millis();

    // 姿勢
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    float roll  = euler.x();
    float pitch = euler.y();
    float yaw   = euler.z();

    // ロードセルの生値（相対値として使う）
    long rawL = readRaw(scaleLeft, 3);
    long rawR = readRaw(scaleRight, 3);

    // Unityで扱いやすいように float に変換
    float loadL = (float)rawL;
    float loadR = (float)rawR;

    //OscWiFi.send(host, port, "/sensor", roll, pitch, yaw, loadL, loadR);

    Serial.printf("Send /sensor R=%.2f P=%.2f Y=%.2f L=%.0f R=%.0f\n",
                  roll, pitch, yaw, loadL, loadR);
  }

  //OscWiFi.update();
}

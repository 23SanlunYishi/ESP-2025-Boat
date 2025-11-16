#include <ArduinoOSCWiFi.h>
#include <SPIFFS.h>
#include <CANFDController.h>

// WiFi関係の設定
const char* ssid = "D2-Score-2.4GHz";  // WiFiのSSID
const char* password = "robomech-d2";  // WiFiのパスワード
const IPAddress ip(192, 168, 11, 70); // 固定IPアドレス(自分で設定した)
const IPAddress gateway(192, 168, 11, 1); // ゲートウェイアドレス(ルーターのIPアドレス)
const IPAddress subnet(255, 255, 255, 0);  // サブネットマスク
const int UDP_PORT = 8000;

// canfd関係の設定
#define SW1 21
CANFDController canfd(5, 33);
CANFDMessage frame;

//LoadSWの設定
const int pwmPin = 15;      // 出力するピン
const int pwmChannel = 0;   // 0〜7のチャンネル
const int pwmFreq = 25000;  // 25kHz
const int pwmResolution = 10; // 10 bit（0〜1023）

// その他のグローバル変数
float air = 0.0;

void setup() {
  Serial.begin(115200);

  // WiFi接続の初期化
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // OSCの初期化
  OscWiFi.subscribe(UDP_PORT, "/air", [](OscMessage& m)
  {
    air = m.arg<float>(0);

    Serial.print("Recv /air : ");
    Serial.print("air="); Serial.println(air);
  });

  // CANFDの初期化
  canfd.setup();
  pinMode(SW1, INPUT);
  for (int i = 0; i < 16; ++i)
    frame.dataFloat[i] = 0;
    
  // LoadSWの起動
  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(pwmPin, pwmChannel);
  ledcWrite(pwmChannel, (int)(1.15/3.3*1023.0));  // 1.15Vを出力
}

void loop() {
  OscWiFi.update();

  canfd.send(frame);
  canfd.receive();
  frame.dataFloat[1] = air;
}
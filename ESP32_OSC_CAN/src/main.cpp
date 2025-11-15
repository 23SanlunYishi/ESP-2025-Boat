#include <ArduinoOSCWiFi.h>
#include <SPIFFS.h>
#include <CANFDController.h>

// WiFi関係の設定
const char* ssid = "Buffalo-G-23D6";  // WiFiのSSID
const char* password = "FuEjan9m5Sp4";  // WiFiのパスワード
const IPAddress ip(192, 168, 1, 70); // 固定IPアドレス(自分で設定した)
const IPAddress gateway(192, 168, 11, 1); // ゲートウェイアドレス(ルーターのIPアドレス)
const IPAddress subnet(255, 255, 255, 0);  // サブネットマスク
const int UDP_PORT = 8000;
int i; float f; String s;


// canfd関係の設定
#define SW1 21
CANFDController canfd(5, 33);
CANFDMessage frame;

// その他のグローバル変数
bool state = false;

void setup() {
  Serial.begin(115200);

  // WiFi接続の初期化
  // WiFi.begin(ssid, password);
  // WiFi.config(ip, gateway, subnet);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting to WiFi...");
  // }
  // Serial.println("Connected to WiFi");
  
  // UDPの初期化
  // Serial.println(WiFi.localIP());
  // udp.begin(UDP_PORT);
  // Serial.printf("Starting UDP on port %d\n", UDP_PORT);

  // OscWiFi.subscribe(UDP_PORT, "/bind/values", i, f, s);

  // CANFDの初期化
  canfd.setup();
  pinMode(SW1, INPUT);
  for (int i = 0; i < 16; ++i)
    frame.dataFloat[i] = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  // char incomingPacket[255];
  // int packetSize = udp.parsePacket();
  // if (packetSize) {
  //   int len = udp.read(incomingPacket, 255);
  //   if (len > 0) {
  //     incomingPacket[len] = 0;
  //   }
  //   Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
  //   Serial.printf("Data: %s\n", incomingPacket);
    
    
  // }
  // OscWiFi.update();
  // Serial.println("");

  canfd.send(frame);
  canfd.receive();
  if(state) {
    frame.dataFloat[1] = 255;
  } else {
    frame.dataFloat[1] = 0;
  }
  state = !state;
  delay(1000);
}
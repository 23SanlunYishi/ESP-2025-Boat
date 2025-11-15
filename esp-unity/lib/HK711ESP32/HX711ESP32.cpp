#include <Arduino.h>
#include "HX711.h"

// 接続ピン（必要に応じて変更OK）
#define HX711_DT  19  // DOUT
#define HX711_SCK 18  // SCK

HX711 scale;

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 計測開始");

  // HX711 初期化
  scale.begin(HX711_DT, HX711_SCK);

  // ゼロ点（何も載せずに）
  Serial.println("ゼロ点を設定中...");
  scale.tare();  
  Serial.println("完了。");

  // キャリブレーション値（仮の値）
  // → 実際には既知の重りで調整が必要
  scale.set_scale(2280.f); // 例：キャリブ係数
}

void loop() {
  // 単位付きで重量を取得（グラム換算）
  float weight = scale.get_units(5); // 平均5回
  Serial.print("Weight: ");
  Serial.print(weight, 2);  // 小数点2桁表示
  Serial.println(" g");
  delay(500);
}
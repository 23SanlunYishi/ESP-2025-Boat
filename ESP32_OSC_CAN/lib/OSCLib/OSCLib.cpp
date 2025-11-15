#pragma once

#include "OSCLib.hpp"
#include <Arduino.h>

// 受信メッセージを解析して構造体を返す
OscMessage parseOscMessage(char* packet, int packetSize) {
  OscMessage message;

  // アドレス部分の抽出
  String address = String(packet);
  Serial.println("Received OSC address: " + address);

  // アドレス部分の終わりを見つける（4バイト境界に揃える）
  int addressEnd = padSize(address.length() + 1);

  // 型タグの位置を見つける
  int typeTagPos = addressEnd;

  // 型タグが存在し、"i"（整数）であることを確認
  if (packet[typeTagPos] == ',' && packet[typeTagPos + 1] == 'i') {
    // 値の位置を取得して整数として読み取る（型タグの4バイト後が値の開始位置）
    int valuePos = padSize(typeTagPos + 2);
    memcpy(&message.intValue, packet + valuePos, sizeof(message.intValue));
    message.intValue = ntohl(message.intValue);  // ネットワークバイトオーダーをホストバイトオーダーに変換
    message.isInt = true;
    Serial.println("Received int value: " + String(message.intValue));
  } 
  // 型タグが存在し、"f"（浮動小数点）であることを確認
  else if (packet[typeTagPos] == ',' && packet[typeTagPos + 1] == 'f') {
    int valuePos = padSize(typeTagPos + 2);
    uint32_t networkOrderValue;
    memcpy(&networkOrderValue, packet + valuePos, sizeof(networkOrderValue));
    networkOrderValue = ntohl(networkOrderValue);
    memcpy(&message.floatValue, &networkOrderValue, sizeof(message.floatValue));
    message.isFloat = true;
    Serial.println("Received float value: " + String(message.floatValue));
  }
  // 型タグが存在し、"s"（文字列）であることを確認
  else if (packet[typeTagPos] == ',' && packet[typeTagPos + 1] == 's') {
    int valuePos = padSize(typeTagPos + 2);
    strcpy(message.stringValue, (char*)(packet + valuePos));
    message.isString = true;
    Serial.println("Received string value: " + String(message.stringValue));
  } 
  else {
    Serial.println("Unexpected or missing type tag");
  }

  return message;
}
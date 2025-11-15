#pragma once

#include <OSCMessage.h>

struct OscMessage {
  bool isInt = false;
  bool isFloat = false;
  bool isString = false;
  int intValue = 0;
  float floatValue = 0.0f;
  char stringValue[256] = {0};
};

OscMessage parseOscMessage(char* packet, int packetSize);
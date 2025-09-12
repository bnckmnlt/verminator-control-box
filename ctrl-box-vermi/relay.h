#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>
#include <EEPROM.h>

extern int relayPins[2][4];

namespace relay {

  constexpr int RELAY_EEPROM_BASE = 10;
  
  template <typename T, size_t N>
  constexpr size_t NUM_ELEMENTS(const T (&)[N]) {
    return N;
  }

  void begin();
  void saveState(int board, int relay, int state);
  int getAddress(int board, int relay);
  int getState(int board, int relay);
  void triggerRelay(int boardNumber, int relayNumber, int relayTrigger);
}

#endif
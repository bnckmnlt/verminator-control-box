#ifndef MISTING_H
#define MISTING_H

#include <Arduino.h>
#include <limits.h>

namespace misting {
  extern unsigned long manualMistingTimestamp;
  extern unsigned long manualTimeout;

  constexpr int EEPROM_MISTING_ACTIVE  = 10 + 120;
  constexpr int EEPROM_MISTING_TIMEOUT = 10 + 121;

  void restoreState();
  void control();
  void setConfig(bool state, unsigned long durationMins, bool indefinite);
}

#endif
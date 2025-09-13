#ifndef AERATION_H
#define AERATION_H

#include <Arduino.h>
#include <limits.h>

namespace aeration {
  extern unsigned long manualFanTimestamp;
  extern unsigned long manualTimeout;

  constexpr int EEPROM_AERATION_ACTIVE  = 10 + 110;
  constexpr int EEPROM_AERATION_TIMEOUT = 10 + 111;

  void restoreState();
  void control();
  void setConfig(bool state, unsigned long durationMins, bool indefinite);
}

#endif

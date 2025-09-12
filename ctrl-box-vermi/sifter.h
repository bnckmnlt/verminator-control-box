#ifndef SIFTER_H
#define SIFTER_H

#include <Arduino.h>
#include <limits.h>

namespace sifter {
  extern unsigned long manualSifterTimestamp;
  extern unsigned long manualTimeout;

  constexpr int EEPROM_SIFTER_ACTIVE  = 10 + 100;
  constexpr int EEPROM_SIFTER_TIMEOUT = 10 + 101;

  void restoreState();
  void control();
  void setConfig(bool state, unsigned long durationMins, bool indefinite);
}

#endif

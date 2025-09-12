#ifndef VERMITEA_H
#define VERMITEA_H

#include <Arduino.h>
#include <limits.h>

namespace vermitea {
  extern unsigned long manualVermiteaTimestamp;
  extern unsigned long manualTimeout;

  constexpr int EEPROM_VERMITEA_ACTIVE  = 10 + 130;
  constexpr int EEPROM_VERMITEA_TIMEOUT = 10 + 131;

  void restoreState();
  void control();
  void setConfig(bool state, unsigned long durationMins, bool indefinite);
}

#endif
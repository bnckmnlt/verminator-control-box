#ifndef SOIL_H
#define SOIL_H

#include <EEPROM.h>

namespace soil {

  constexpr int PUMP_STATUS_ADDR  = 3;
  constexpr int PUMP_STATE_ADDR   = 4;

  constexpr int SOIL_MOISTURE_PIN =  61;

  extern int moisture;
  extern int lastValid;

  enum PumpMode { AUTO, MANUAL };
  extern PumpMode pumpStatus;  

  extern bool pumpState;

  void read();
  void control();
  void setMode(bool state);
  void begin();
}

#endif

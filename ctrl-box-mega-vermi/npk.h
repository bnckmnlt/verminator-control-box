#ifndef NPK_H
#define NPK_H

#include <Arduino.h>
#include <ModbusMaster.h>

extern ModbusMaster node;

namespace npk {
  constexpr int NITROGEN_ADDR       = 0x1E;
  constexpr int PHOSPHORUS_ADDR     = 0x1F;
  constexpr int POTASSIUM_ADDR      = 0x20;

  constexpr int MAX485_DE           = 7;
  constexpr int MAX485_RE_NEG       = 6;

  constexpr int MIN_NPK_VALUE       = 0;
  constexpr int MAX_NPK_VALUE       = 2000;

  constexpr int MAX_N_AGRO_VALUE    = 15000;
  constexpr int MAX_PK_AGRO_VALUE   = 5000;

  const float P2O5_TO_P = 0.4364f;
  const float K2O_TO_K  = 0.8301f;

  extern int nitrogen;
  extern int phosphorus;
  extern int potassium;
  extern int nitrogenPct;
  extern int phosphorusPct;
  extern int potassiumPct;
  extern int nitrogenAgronomicPct;
  extern int phosphorusAgronomicPct;
  extern int potassiumAgronomicPct;

  void setup();
  void preTransmission();
  void postTransmission();
  void read();
  float convertToPercentage(int value, int MAX_VALUE);
  float convertToAgrnomicPercentage(int value);
  void handleError(uint8_t errNum);
}

#endif
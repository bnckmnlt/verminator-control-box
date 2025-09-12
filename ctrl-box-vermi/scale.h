
#ifndef SCALE_H
#define SCALE_H

#include <Arduino.h>
#include <EEPROM.h>
#include "HX711.h"
#include <HX711_ADC.h>

extern HX711_ADC scaleCompost, scaleJuice, scaleWater;

namespace scale {
  constexpr int DT_COMPOST    = 22;
  constexpr int SCK_COMPOST   = 23;
  constexpr int DT_JUICE      = 24;
  constexpr int SCK_JUICE     = 25;
  constexpr int DT_WATER      = 26;
  constexpr int SCK_WATER     = 27;
  
  extern float compostKilo;
  extern float juiceLiters;
  extern float waterLiters;

  extern bool scaleCalMode;
  extern bool resumeCalibration;
  extern bool proceed;
  extern bool saveValue;

  extern float knownMass;

  enum ScaleID {
    SCALE_COMPOST,
    SCALE_JUICE,
    SCALE_WATER
  };

  struct Calibration {
    float calibrationFactor;
    long zeroFactor;
  };

  extern ScaleID calibrationId;
  
  extern Calibration compostCal, juiceCal, waterCal;

  void begin();
  int getAddress(ScaleID id);
  void loadCalibration(ScaleID id, Calibration &calib);
  void saveCalibration(ScaleID id, const Calibration &calib);
  void setup(ScaleID id, HX711_ADC &scale, const char *label, Calibration &calib);
  void calibrate(ScaleID id);
  void read(HX711_ADC &scale, float &litersVar, float maxLiters, const char *label);
  ScaleID parseScaleID(const char* token);
  HX711_ADC* getScale(ScaleID id);
  Calibration* getCalibration(ScaleID id);
  const char* getLabel(ScaleID id);
}

#endif

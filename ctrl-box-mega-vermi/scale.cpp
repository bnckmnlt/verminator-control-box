#include "scale.h"

HX711_ADC scaleCompost(scale::DT_COMPOST, scale::SCK_COMPOST), scaleJuice(scale::DT_JUICE, scale::SCK_JUICE), scaleWater(scale::DT_WATER, scale::SCK_WATER);

namespace scale {

  static const int EEPROM_SCALE_BASE = 100;
  static const int EEPROM_SCALE_SIZE = sizeof(Calibration);

  float compostKilo;
  float juiceLiters;
  float waterLiters;

  bool scaleCalMode       = false;
  bool resumeCalibration  = false;
  bool proceed            = false;
  bool saveValue          = false;

  float knownMass = 0;

  ScaleID calibrationId;

  Calibration compostCal, juiceCal, waterCal;

  void begin() {
    loadCalibration(SCALE_COMPOST, compostCal);
    loadCalibration(SCALE_JUICE, juiceCal);
    loadCalibration(SCALE_WATER, waterCal);

    scaleCompost.begin();
    scaleJuice.begin();
    scaleWater.begin();

    scaleCompost.start(2000, false);
    scaleJuice.start(2000, false);
    scaleWater.start(2000, false);

    setup(SCALE_COMPOST, scaleCompost, "Compost", compostCal);
    setup(SCALE_JUICE, scaleJuice, "Juice", juiceCal);
    setup(SCALE_WATER, scaleWater, "Water", waterCal);
  }

  int getAddress(ScaleID id) {
    return EEPROM_SCALE_BASE + static_cast<int>(id) * EEPROM_SCALE_SIZE;
  }

  void loadCalibration(ScaleID id, Calibration &calib) {
    int addr = getAddress(id);
    EEPROM.get(addr, calib);
    if (isnan(calib.calibrationFactor) || calib.calibrationFactor == 0.0f) {
      calib.calibrationFactor = 1.0f;
      calib.zeroFactor = 0;
    }
  }

  void saveCalibration(ScaleID id, const Calibration &calib) {
    int addr = getAddress(id);
    EEPROM.put(addr, calib);
  }

  void setup(ScaleID id, HX711_ADC &scale, const char *label, Calibration &calib) {
    loadCalibration(id, calib);

    char buff[100];
    sprintf(buff, "<info: HX711: %s → CF: %.2f, Offset: %ld>", label, calib.calibrationFactor, calib.zeroFactor);
    Serial.println(buff);

    scale.setTareOffset(calib.zeroFactor);
    delay(500);
    scale.setCalFactor(calib.calibrationFactor);
  }

  void calibrate(ScaleID id) {
    HX711_ADC* scale = getScale(id);
    Calibration &calib = *getCalibration(id);
    const char* label = scale::getLabel(id);

    Serial.println("<info: HX711: Calibration starting>");
    Serial.println("Remove ALL weight applied to the Load Cell. Send 'continue' when ready.");

    resumeCalibration = false;

    while (!resumeCalibration) {
      scale->update();
      if (proceed) {
        proceed = false;
        scale->tareNoDelay();
      }
      if (scale->getTareStatus()) {
        resumeCalibration = true;
      }
    }

    Serial.println("<info: Tare complete. Place your known mass on the loadcell>");

    resumeCalibration = false;
    while (!resumeCalibration) {
      scale->update();
      if (knownMass != 0) {
        resumeCalibration = true;
      }
    }
    
    scale->refreshDataSet();
    float newCalFactor = scale->getNewCalibration(knownMass);

    calib.zeroFactor = scale->getTareOffset();
    calib.calibrationFactor = newCalFactor;

    char buff[100];
    sprintf(buff, "<info: HX711: New Calibration for %s → CF: %.2f, Offset: %ld>", label, calib.calibrationFactor, calib.zeroFactor);
    Serial.println(buff);

    Serial.println("<info: HX711: Save values to EEPROM Address?>");

    resumeCalibration = false;

    while (!resumeCalibration) {
      if (saveValue) {
        saveValue = false;
  #if defined(ESP8266) || defined(ESP32)
        EEPROM.begin(512);
  #endif
        saveCalibration(id, calib);
  #if defined(ESP8266) || defined(ESP32)
        EEPROM.commit();
  #endif
        Serial.println("<info: Calibration values saved>");
        resumeCalibration = false;
        scaleCalMode = false;
      }
    }
  }

  void read(HX711_ADC &scale, float &litersVar, float maxLiters, const char *label) {
    scale.update();
    litersVar = constrain(scale.getData() / 1000.0, 0.0, maxLiters);
  }

  ScaleID parseScaleID(const char* token) {
    if (strcmp(token, "COMPOST") == 0) return SCALE_COMPOST;
    if (strcmp(token, "JUICE") == 0)   return SCALE_JUICE;
    if (strcmp(token, "WATER") == 0)   return SCALE_WATER;
    return SCALE_COMPOST;
  }

  HX711_ADC* getScale(ScaleID id) {
    switch(id) {
      case SCALE_COMPOST: return &scaleCompost;
      case SCALE_JUICE: return &scaleJuice;
      case SCALE_WATER: return &scaleWater;
      default: return nullptr;
    }
  }

  Calibration* getCalibration(ScaleID id) {
    switch(id) {
      case SCALE_COMPOST: return &compostCal;
      case SCALE_JUICE: return &juiceCal;
      case SCALE_WATER: return &waterCal;
      default: return nullptr;
    }
  }

  const char* getLabel(ScaleID id) {
    switch(id) {
      case SCALE_COMPOST: return "Compost";
      case SCALE_JUICE: return "Juice";
      case SCALE_WATER: return "Water";
      default: return "Unknown";
    }
  }
}
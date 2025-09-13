#ifndef ENV_H
#define ENV_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <limits.h>

extern Adafruit_BME280 bme;

namespace env {
  constexpr int EEPROM_FAN_STATUS_ADDR      =  0;
  constexpr int EEPROM_FAN_STATE_ADDR       =  1;
  constexpr int EEPROM_MANUAL_TIMEOUT_ADDR  =  2;

  const int maxSafeTemp     = 30;
  const int fanOffTemp      = 28;

  const int maxSafeHumid    = 85;
  const int fanOffHumid     = 80;
  
  const int tempThreshold   = 35;
  const int humidThreshold  = 40;

  extern float temperature;
  extern float humidity;

  enum FanMode { AUTO, MANUAL };
  extern FanMode fanStatus; 

  extern bool fanState;

  extern unsigned long manualFanTimestamp;
  extern unsigned long manualTimeout;

  bool begin();
  void saveFanState();
  void loadFanState();
  void read();
  void control();
  void setMode(bool state, unsigned long durationMins, bool indefinite);
}

#endif
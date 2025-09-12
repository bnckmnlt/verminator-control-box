#include "soil.h"
#include "relay.h"
#include "env.h"

namespace soil {
  int moisture        = 0;
  int lastValid       = 45;

  PumpMode pumpStatus = AUTO;
  bool pumpState      = false;

  const int   WET_RAW     = 256;
  const int   DRY_RAW     = 535;
  const int   NUM_SAMPLES = 10;
  const int   READ_MIN    = 252;
  const int   READ_MAX    = 532;

  void read() {
    int validReads = 0;
    long sum = 0;

    for (int i = 0; i < NUM_SAMPLES; i++) {
      int r = analogRead(SOIL_MOISTURE_PIN);
      if (r >= READ_MIN && r <= READ_MAX) {
        sum += r;
        validReads++;
      }
      delay(10);
    }

    if (validReads > 0) {
      int avg = sum / validReads;

      int m = map(avg, WET_RAW, DRY_RAW, 100, 0);
      moisture = constrain(m, 0, 100);
      lastValid = moisture;
    } else {
      moisture = lastValid;
      Serial.println("<warn: Using last valid moisture>");
    }
  }

  void control() {
    if (moisture > 60) {
      if (pumpState) {
        pumpState = false;
        relay::triggerRelay(0, 2, LOW);
        Serial.println("<info: Soil Misting off – soil is saturated already>");
      }
      pumpStatus = AUTO;
    } 
    else {
      if (pumpStatus == AUTO) {
        bool shouldRun = (moisture < 40 && env::temperature > env::tempThreshold && env::humidity < env::humidThreshold);
        if (pumpState != shouldRun) {
          relay::triggerRelay(0, 2, shouldRun ? HIGH : LOW);
          pumpState = shouldRun;
          Serial.print("<info: Soil Misting ");
          Serial.print(shouldRun ? "ON" : "OFF");
          Serial.println(">");
        }
      }
    }

    EEPROM.update(PUMP_STATE_ADDR, pumpState);
    EEPROM.update(PUMP_STATUS_ADDR, pumpStatus);
  }

  void setMode(bool state) {
    pumpStatus = MANUAL;
    pumpState = state;

    relay::triggerRelay(0, 2, state);

    EEPROM.update(PUMP_STATUS_ADDR, pumpStatus);
    EEPROM.update(PUMP_STATE_ADDR, pumpState);

    Serial.print("<info: Soil Misting turned ");
    Serial.print(state ? "ON" : "OFF");
    Serial.println(" manually>");
  }

  void begin() {
    pumpStatus = (PumpMode)EEPROM.read(PUMP_STATUS_ADDR);
    pumpState = EEPROM.read(PUMP_STATE_ADDR);
    
    if (pumpStatus == MANUAL) {
      relay::triggerRelay(0, 2, pumpState);
    }
  }
}

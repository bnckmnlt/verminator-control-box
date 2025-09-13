#include "misting.h"
#include "relay.h"

namespace misting {
  unsigned long manualMistingTimestamp  = 0;
  unsigned long manualTimeout           = 0;
  bool mistingState                     = false;

  void restoreState() {
    byte active = EEPROM.read(EEPROM_MISTING_ACTIVE);
    if (active == 1) {
      unsigned long remaining;
      EEPROM.get(EEPROM_MISTING_TIMEOUT, remaining);

      manualMistingTimestamp = millis();
      manualTimeout = remaining;
      mistingState = true;

      relay::triggerRelay(1, 1, 1);
      Serial.print("<info: Restored Conveyor Misting to run for ");
      Serial.print(remaining / 60000UL);
      Serial.println(" minute(s)>");
    }
  }

  void setConfig(bool state, unsigned long durationMins, bool indefinite) {
    mistingState = state;
    manualMistingTimestamp = millis();
    manualTimeout = indefinite ? ULONG_MAX : durationMins * 60UL * 1000UL;

    EEPROM.update(EEPROM_MISTING_ACTIVE, state ? 1 : 0);
    if (state && !indefinite) {
      unsigned long remaining = manualTimeout;
      EEPROM.put(EEPROM_MISTING_TIMEOUT, remaining);
    }

    relay::triggerRelay(1, 1, state);

    Serial.print("<info: Conveyor Misting control turned ");
    Serial.print(state ? "ON" : "OFF");
    if (indefinite && state) {
      Serial.println(" indefinitely>");
    } else if (state) {
      Serial.print(" for ");
      Serial.print(durationMins);
      Serial.println(" minute(s)>");
    } else {
      Serial.println(">");
    }
  }

  void control() {
    if (mistingState && millis() - manualMistingTimestamp >= manualTimeout) {
      relay::triggerRelay(1, 1, 0);
      mistingState = false;

      EEPROM.update(EEPROM_MISTING_ACTIVE, 0);
      unsigned long zero = 0;
      EEPROM.put(EEPROM_MISTING_TIMEOUT, zero);

      Serial.println("<info: Conveyor Misting control timeout reached; turning OFF>");
    }
  }

}
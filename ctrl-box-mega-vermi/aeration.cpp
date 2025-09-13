#include "aeration.h"
#include "relay.h"

namespace aeration {
  unsigned long manualAerationTimestamp = 0;
  unsigned long manualTimeout      = 0;
  bool aeration                    = false;

  void restoreState() {
    byte active = EEPROM.read(EEPROM_AERATION_ACTIVE);
    if (active == 1) {
      unsigned long remaining;
      EEPROM.get(EEPROM_AERATION_TIMEOUT, remaining);

      manualAerationTimestamp = millis();
      manualTimeout = remaining;
      aeration = true;

      relay::triggerRelay(1, 0, 1);
      Serial.print("<info: Restored Soil Aeration to run for ");
      Serial.print(remaining / 60000UL);
      Serial.println(" minute(s)>");
    }
  }

  void setConfig(bool state, unsigned long durationMins, bool indefinite) {
    aeration = state;
    manualAerationTimestamp = millis();
    manualTimeout = indefinite ? ULONG_MAX : durationMins * 60UL * 1000UL;

    EEPROM.update(EEPROM_AERATION_ACTIVE, state ? 1 : 0);
    if (state && !indefinite) {
      EEPROM.put(EEPROM_AERATION_TIMEOUT, manualTimeout);
    }

    relay::triggerRelay(1, 0, state);

    Serial.print("<info: Soil Aeration control turned ");
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
    if (aeration && millis() - manualAerationTimestamp >= manualTimeout) {
      relay::triggerRelay(1, 0, 0);
      aeration = false;

      EEPROM.update(EEPROM_AERATION_ACTIVE, 0);
      unsigned long zero = 0;
      EEPROM.put(EEPROM_AERATION_TIMEOUT, zero);

      Serial.println("<info: Soil Aeration control timeout reached; turning OFF>");
    }
  }

}

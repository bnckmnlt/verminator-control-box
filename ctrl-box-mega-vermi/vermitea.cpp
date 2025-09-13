#include "vermitea.h"
#include "relay.h"

namespace vermitea {
  unsigned long manualVermiteaTimestamp  = 0;
  unsigned long manualTimeout            = 0;
  bool vermiteaState                     = false;

  void restoreState() {
    byte active = EEPROM.read(EEPROM_VERMITEA_ACTIVE);
    if (active == 1) {
      unsigned long remaining;
      EEPROM.get(EEPROM_VERMITEA_TIMEOUT, remaining);

      manualVermiteaTimestamp = millis();
      manualTimeout = remaining;
      vermiteaState = true;

      relay::triggerRelay(1, 1, 1);
      Serial.print("<info: Restored Vermitea Pump to run for ");
      Serial.print(remaining / 60000UL);
      Serial.println(" minute(s)>");
    }
  }

  void setConfig(bool state, unsigned long durationMins, bool indefinite) {
    vermiteaState = state;
    manualVermiteaTimestamp = millis();
    manualTimeout = indefinite ? ULONG_MAX : durationMins * 60UL * 1000UL;

    EEPROM.update(EEPROM_VERMITEA_ACTIVE, state ? 1 : 0);
    if (state && !indefinite) {
      unsigned long remaining = manualTimeout;
      EEPROM.put(EEPROM_VERMITEA_TIMEOUT, remaining);
    }

    relay::triggerRelay(0, 3, state);

    Serial.print("<info: Vermitea Pump control turned ");
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
    if (vermiteaState && millis() - manualVermiteaTimestamp >= manualTimeout) {
      relay::triggerRelay(1, 1, 0);
      vermiteaState = false;

      EEPROM.update(EEPROM_VERMITEA_ACTIVE, 0);
      unsigned long zero = 0;
      EEPROM.put(EEPROM_VERMITEA_TIMEOUT, zero);

      Serial.println("<info: Vermitea Pump control timeout reached; turning OFF>");
    }
  }

}
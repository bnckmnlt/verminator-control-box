#include "sifter.h"
#include "relay.h"

namespace sifter {
  unsigned long manualSifterTimestamp = 0;
  unsigned long manualTimeout         = 0;
  bool sifterState                    = false;

  void restoreState() {
    byte active = EEPROM.read(EEPROM_SIFTER_ACTIVE);
    if (active == 1) {
      unsigned long remaining;
      EEPROM.get(EEPROM_SIFTER_TIMEOUT, remaining);

      manualSifterTimestamp = millis();
      manualTimeout = remaining;
      sifterState = true;

      relay::triggerRelay(1, 1, 1);
      Serial.print("<info: Restored Sifter to run for ");
      Serial.print(remaining / 60000UL);
      Serial.println(" minute(s)>");
    }
  }

  void setConfig(bool state, unsigned long durationMins, bool indefinite) {
    sifterState = state;
    manualSifterTimestamp = millis();
    manualTimeout = indefinite ? ULONG_MAX : durationMins * 60UL * 1000UL;

    EEPROM.update(EEPROM_SIFTER_ACTIVE, state ? 1 : 0);
    if (state && !indefinite) {
      unsigned long remaining = manualTimeout;
      EEPROM.put(EEPROM_SIFTER_TIMEOUT, remaining);
    }

    relay::triggerRelay(1, 1, state);

    Serial.print("<info: Sifter control turned ");
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
    if (sifterState && millis() - manualSifterTimestamp >= manualTimeout) {
      relay::triggerRelay(1, 1, 0);
      sifterState = false;

      EEPROM.update(EEPROM_SIFTER_ACTIVE, 0);
      unsigned long zero = 0;
      EEPROM.put(EEPROM_SIFTER_TIMEOUT, zero);

      Serial.println("<info: Sifter control timeout reached; turning OFF>");
    }
  }

}

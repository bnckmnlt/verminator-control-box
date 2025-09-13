#include "env.h"
#include "relay.h"

Adafruit_BME280 bme;

namespace env {
  float temperature = 0.0;
  float humidity    = 0.0;

  FanMode fanStatus = AUTO;
  bool fanState     = false;

  unsigned long manualFanTimestamp  = 0;
  unsigned long manualTimeout       = 10UL * 60UL * 1000UL;

  bool begin() {
    bool status = bme.begin(0x76);

    loadFanState();

    return status;
  }

  void saveFanState() {
    EEPROM.update(EEPROM_FAN_STATUS_ADDR, static_cast<byte>(fanStatus));
    EEPROM.update(EEPROM_FAN_STATE_ADDR, fanState);
    EEPROM.put(EEPROM_MANUAL_TIMEOUT_ADDR, manualTimeout);
  }

  void loadFanState() {
    fanStatus = static_cast<FanMode>(EEPROM.read(EEPROM_FAN_STATUS_ADDR));
    fanState = EEPROM.read(EEPROM_FAN_STATE_ADDR);
    EEPROM.get(EEPROM_MANUAL_TIMEOUT_ADDR, manualTimeout);

    relay::triggerRelay(0, 0, fanState);
    relay::triggerRelay(0, 1, fanState);
  }

  void read() {
    temperature = (int)constrain(bme.readTemperature(), -40, 85);
    humidity    = (int)constrain(bme.readHumidity(), 0, 100);
  }

  void control() {
    if (fanStatus == AUTO) {
      if ((temperature > maxSafeTemp || humidity > maxSafeHumid) && !fanState) {
        relay::triggerRelay(0, 0, 1);
        relay::triggerRelay(0, 1, 1);
        Serial.print("<warn: Bedding layer environment is outside recommended range – Temp: ");
        Serial.print(temperature);
        Serial.print("°C, Humidity: ");
        Serial.print(humidity);
        Serial.println("%>");
        fanState = true;
      } 
      else if ((temperature < fanOffTemp && humidity < fanOffHumid) && fanState) {
        relay::triggerRelay(0, 0, 0);
        relay::triggerRelay(0, 1, 0);
        fanState = false;
      }
    } else {
      if ((millis() - manualFanTimestamp) >= manualTimeout) {
        fanStatus = AUTO;
        Serial.println("<info: Fan mode reverted to AUTO>");
      }
    }

    saveFanState();
  }

  void setMode(bool state, unsigned long durationMins, bool indefinite) {
    fanStatus = MANUAL;
    fanState = state;

    relay::triggerRelay(0, 0, state);
    relay::triggerRelay(0, 1, state);

    manualFanTimestamp = millis();
    manualTimeout = indefinite ? ULONG_MAX : durationMins * 60UL * 1000UL;

    Serial.print("<info: Fan turned ");
    Serial.print(state ? "ON" : "OFF");

    saveFanState();

    if (indefinite) {
      Serial.println(" indefinitely>");
    } else {
      Serial.print(" for ");
      Serial.print(durationMins);
      Serial.println(" minute(s)>");
    }
  }
}
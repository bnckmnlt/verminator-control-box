#include "relay.h"
#include "env.h"
#include "soil.h"
#include "aeration.h"
#include "sifter.h"

int relayPins[2][4] = {
  { 33, 35, 37, 39 }, /** [0][0] - In Fan,        [0][1] - Out Fan 2,    [0][2] - Pump Sprinkler, [0][3] - Pump for Vermitea   **/
  { 32, 34, 36, 38 }, /** [1][0] - Soil Aeration, [1][1] - Top Misting,  [1][2] N/A               [1][3] - N/A                 **/
};

namespace relay {
  int getAddress(int board, int relay) {
    return RELAY_EEPROM_BASE + board * 4 + relay;
  }

  void saveState(int board, int relay, int state) {
    EEPROM.update(getAddress(board, relay), state);
  }

  int getState(int board, int relay) {
    return EEPROM.read(getAddress(board, relay));
  }

  void begin() {
    for (int i = 0; i < NUM_ELEMENTS(relayPins); i++) {
      for (int j = 0; j < (sizeof(relayPins[i]) / sizeof(relayPins[i][0])); j++) {
        pinMode(relayPins[i][j], OUTPUT);
        digitalWrite(relayPins[i][j], HIGH);
      }
    }

    /** RESTORE PIN STATE **/
    for (int board = 0; board < 2; board++) {
      for (int relay = 0; relay < 4; relay++) {
        int storedState = getState(board, relay);
        int pin = relayPins[board][relay];

        pinMode(pin, OUTPUT);
        digitalWrite(pin, storedState == 1 ? LOW : HIGH);

        if (pin == 41 || pin == 40) {
          env::fanState = (storedState == 1);
        } else if (pin == 42) {
          soil::pumpState = (storedState == 1);
        }

        Serial.print("<info: Restored relay ");
        Serial.print(relay);
        Serial.print(" on board ");
        Serial.print(board);
        Serial.print(" to state ");
        Serial.print(storedState);
        Serial.println(">");
      }
    }

    aeration::restoreState();
    sifter::restoreState();
  }

  void triggerRelay(int board, int relay, int state) {
    if (board < 0 || board >= 2 || relay < 0 || relay >= 4) {
      Serial.println("<error: Invalid board or relay number>");
      return;
    }

    int pin = relayPins[board][relay];
    if (pin == -1) {
      Serial.println("<error: Unassigned relay pin>");
      return;
    }

    char msg[50];
    sprintf(msg, "<info: Relay FB:%d:%d:%d>", board, relay, state);
    Serial.println(msg);

    bool isOn = (state == 1);
    digitalWrite(pin, isOn ? LOW : HIGH);
    saveState(board, relay, isOn ? 1 : 0);
  }
}
#include "npk.h"

ModbusMaster node;

namespace npk {
  int nitrogen;
  int phosphorus;
  int potassium;
  int nitrogenPct;
  int phosphorusPct;
  int potassiumPct;

  void preTransmission() {
    digitalWrite(MAX485_RE_NEG, HIGH);
    digitalWrite(MAX485_DE, HIGH);
  }

  void postTransmission() {
    digitalWrite(MAX485_RE_NEG, LOW);
    digitalWrite(MAX485_DE, LOW);
  }

  void setup() {
    Serial2.begin(4800);

    pinMode(MAX485_RE_NEG, OUTPUT);
    pinMode(MAX485_DE, OUTPUT);
    digitalWrite(MAX485_RE_NEG, LOW);
    digitalWrite(MAX485_DE, LOW);

    node.begin(1, Serial2);
    node.preTransmission(preTransmission);
    node.postTransmission(postTransmission);
  }

  float convertToPercentage(int value) {
    if (value == -1) return -1;
    return ceil(((float)(value - MIN_NPK_VALUE) / (MAX_NPK_VALUE - MIN_NPK_VALUE)) * 100);
  }

  void read() {
    uint8_t result;

    result = node.readHoldingRegisters(NITROGEN_ADDR, 1);
    nitrogen = (result == node.ku8MBSuccess) ? constrain(node.getResponseBuffer(0x0), MIN_NPK_VALUE, MAX_NPK_VALUE) : 0;

    result = node.readHoldingRegisters(PHOSPHORUS_ADDR, 1);
    phosphorus = (result == node.ku8MBSuccess) ? constrain(node.getResponseBuffer(0x0), MIN_NPK_VALUE, MAX_NPK_VALUE) : 0;

    result = node.readHoldingRegisters(POTASSIUM_ADDR, 1);
    potassium = (result == node.ku8MBSuccess) ? constrain(node.getResponseBuffer(0x0), MIN_NPK_VALUE, MAX_NPK_VALUE) : 0;

    nitrogenPct = convertToPercentage(nitrogen);
    phosphorusPct = convertToPercentage(phosphorus) * P2O5_TO_P;
    potassiumPct = convertToPercentage(potassium) * K2O_TO_K;
  }

  void handleError(uint8_t errNum) {
    switch (errNum) {
      case node.ku8MBSuccess: Serial.println(F("<info: Success>")); break;
      case node.ku8MBIllegalFunction: Serial.println(F("<error: Illegal Function>")); break;
      case node.ku8MBIllegalDataAddress: Serial.println(F("<error: Illegal Data Address>")); break;
      case node.ku8MBIllegalDataValue: Serial.println(F("<error: Illegal Data Value>")); break;
      case node.ku8MBSlaveDeviceFailure: Serial.println(F("<fatal: Slave Device Failure>")); break;
      case node.ku8MBInvalidSlaveID: Serial.println(F("<warning: Invalid Slave ID>")); break;
      case node.ku8MBInvalidFunction: Serial.println(F("<warning: Invalid Function>")); break;
      case node.ku8MBResponseTimedOut: Serial.println(F("<warning: Response Timed Out>")); break;
      case node.ku8MBInvalidCRC: Serial.println(F("<warning: Invalid CRC>")); break;
      default: Serial.println(F("<error: Unknown Error>")); break;
    }
  }
}
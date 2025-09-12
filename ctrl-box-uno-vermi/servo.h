#ifndef SERVO_H
#define SERVO_H

#include <Adafruit_TiCoServo.h>

enum DiverterState {
  IDLE,
  EXTENDING,
  HOLDING,
  RETRACTING
};

extern DiverterState diverterState;
void setupDiverter();
void updateDiverter();
void setDiverterState(DiverterState state);
void conveyorHaltTemp();

#endif

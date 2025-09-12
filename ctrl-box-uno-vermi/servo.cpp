#include "servo.h"
#include "stepper.h"

Adafruit_TiCoServo diverterServo;

bool conveyorWasActiveBeforePause = false;
unsigned long servoTimer = 0;
int servoPos = 105;
const int servoMin = 0;
const int servoMax = 105;
const unsigned long stepDelay = 10;

DiverterState diverterState = IDLE;

void setupDiverter() {
  diverterServo.attach(9);
  diverterServo.write(servoPos);
}

void setDiverterState(DiverterState state) {
  if (diverterState == IDLE || diverterState == HOLDING) {
    diverterState = state;
  }
}

void updateDiverter() {
  unsigned long now = millis();

  if (diverterState == EXTENDING) {
    if (now - servoTimer >= stepDelay && servoPos > servoMin) {
      servoPos--;
      diverterServo.write(servoPos);
      servoTimer = now;
      if (servoPos <= servoMin) {
        diverterState = HOLDING;
      }
    }

  } else if (diverterState == RETRACTING) {
    if (servoPos >= servoMax) {
      diverterState = IDLE;
      return;
    }

    if (now - servoTimer >= stepDelay + 5 && servoPos < servoMax) {
      servoPos++;
      diverterServo.write(servoPos);
      servoTimer = now;
      if (servoPos >= servoMax) {
        diverterState = IDLE;
      }
    }
  }
}

void conveyorHaltTemp() {
  if (diverterState == EXTENDING || diverterState == RETRACTING) {
    if (stepper::conveyor.isProcessing) {
      conveyorWasActiveBeforePause = true;
      stepper::conveyor.returningToZero = false;
      stepper::conveyor.isProcessing = false;
      stepper::conveyor.isActive = true;
      stepper::conveyor.stop();
      Serial.println("<Conveyor:Idle>");
    }
  } else if (diverterState == HOLDING || diverterState == IDLE) {
    if (conveyorWasActiveBeforePause && !stepper::conveyor.isProcessing) {
      stepper::conveyor.returningToZero = false;
      stepper::conveyor.receivedSteps = 10000000;
      // stepper::conveyor.receivedSpeed = 500;
      // stepper::conveyor.directionMultiplier = -1;
      digitalWrite(stepper::disableConveyorPin, LOW);
      stepper::conveyor.isProcessing = true;
      stepper::conveyor.rotateRelative();
      Serial.println("<Conveyor:Active>");
      conveyorWasActiveBeforePause = false;
    }
  }
}

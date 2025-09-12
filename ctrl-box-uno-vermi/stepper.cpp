#include "stepper.h"
#include "servo.h"

AccelStepper conveyorStepper(1, 4, 5);
AccelStepper sifterStepper(1, 6, 7);
AccelStepper rakeStepper(1, 2, 3);

namespace stepper {

  StepperController rake(&rakeStepper);
  StepperController conveyor(&conveyorStepper);
  StepperController sifter(&sifterStepper);

  void StepperController::begin() {
    motor->setMaxSpeed(receivedSpeed);
    motor->setAcceleration(receivedAcceleration);
    motor->setCurrentPosition(0);

    motor->disableOutputs();
    digitalWrite(stepper::disableConveyorPin, HIGH);
    digitalWrite(stepper::disableSifterPin, HIGH);
    digitalWrite(stepper::disableRakePin, HIGH);
  }

  void StepperController::rotateRelative() {
    isActive = true;
    motor->setMaxSpeed(receivedSpeed);
    motor->move(directionMultiplier * receivedSteps);
  }

  void StepperController::rotateAbsolute() {
    isActive = true;
    motor->setMaxSpeed(receivedSpeed);
    motor->moveTo(directionMultiplier * receivedSteps);
  }

  void StepperController::returnOrigin() {
    char buff[64];

    if (motor->currentPosition() == 0) {
      snprintf(buff, sizeof(buff), "<info:%s is already at the origin>\n", name);
      Serial.print(buff);
      motor->disableOutputs();
      return;
    }

    isActive = true;
    motor->setMaxSpeed(500);
    motor->moveTo(0);

    snprintf(buff, sizeof(buff), "<info:%s:Returning>\n", name);
    Serial.print(buff);
  }

  void StepperController::stop() {
    motor->stop();
    motor->disableOutputs();
    isActive = false;
  }

  void StepperController::update(int acceleration) {
    isActive = false;
    motor->disableOutputs();
    motor->setAcceleration(acceleration);
  }

  void StepperController::setZero() {
    motor->disableOutputs();
    motor->setCurrentPosition(0);
    Serial.println("<info:Origin position is updated>");
  }

  void StepperController::printPosition() {
    Serial.print("<info:Current Position is ");
    Serial.print(motor->currentPosition());
    Serial.println(">");
  }

  /* Individual Functions */
  void runConveyor() {
    static bool returningToZero = false;

    if (conveyor.isActive) {
      conveyor.motor->enableOutputs();
      conveyor.motor->run();

      if (!returningToZero && conveyor.motor->distanceToGo() == 0 && conveyor.motor->currentPosition() != 0) {
        conveyor.motor->moveTo(0);
        returningToZero = true;
      }

      if (returningToZero && conveyor.motor->currentPosition() == 0) {
        conveyor.isProcessing = false;
        conveyor.isActive = false;
        returningToZero = false;

        Serial.println("<Conveyor:Idle>");
      }
    } else {
      conveyor.motor->disableOutputs();
      conveyor.returningToZero = false;

      return;
    }
  }

  void runSifter() {
    if (sifter.isActive) {
      sifter.motor->enableOutputs();

      sifter.motor->run();

      if (!sifter.returningToZero && sifter.motor->distanceToGo() == 0 && sifter.motor->currentPosition() != 0) {
        sifter.motor->moveTo(0);
        sifter.returningToZero = true;
      }

      if (sifter.returningToZero && sifter.motor->currentPosition() == 0) {
        sifter.isProcessing = false;
        sifter.isActive = false;
        sifter.returningToZero = false;

        Serial.println("<Sifter:Idle>");
      }
    } else {
      sifter.motor->disableOutputs();
      sifter.returningToZero = false;

      return;
    }
  }

  void runRake() {
    if (rake.isActive) {
      rake.motor->enableOutputs();
      rake.motor->run();

      if (rake.rakeIsRunningCommand && rake.motor->distanceToGo() == 0) {
        if (rake.returningToZero) {
          rake.cycleCount++;

          if (rake.cycleCount >= rake.totalCycles) {
            rake.motor->stop();
            rake.motor->disableOutputs();

            rake.rakeIsRunning = false;
            rake.rakeIsRunningCommand = false;
            rake.returningToZero = false;
            rake.cycleCount = 0;

            Serial.println("<Rake:Idle>");
            return;
          }

          rake.motor->moveTo(rake.directionMultiplier * rake.receivedSteps);
          rake.rakeIsRunning = true;
          rake.returningToZero = false;
        } else if (rake.rakeIsRunning) {
          rake.motor->moveTo(0);
          rake.returningToZero = true;
          rake.rakeIsRunning = false;
        }
      }
    } else {
      rake.motor->disableOutputs();
      rake.returningToZero = false;

      return;
    }
  }

}

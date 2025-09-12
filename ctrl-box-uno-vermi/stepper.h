#ifndef STEPPER_H
#define STEPPER_H

#include <AccelStepper.h>

extern AccelStepper rakeStepper, conveyorStepper, sifterStepper;

namespace stepper {

  struct StepperController {
    const char* name;
    AccelStepper* motor;
    long receivedSteps = 0;
    long receivedSpeed = 500;
    long receivedAcceleration = 1000;
    int directionMultiplier = 1;
    bool isActive = false;

    /* Conveyor Control */
    bool isProcessing = false;

    /* Rake Controls */
    int totalCycles = 5;
    int cycleCount = 0;
    bool rakeIsRunning = true;
    bool rakeIsRunningCommand = false;
    bool returningToZero = false;

    StepperController(AccelStepper* m)
      : motor(m) {}

    void begin();
    void rotateRelative();
    void rotateAbsolute();
    void returnOrigin();
    void stop();
    void update(int acceleration);
    void setZero();
    void printPosition();
  };

  extern StepperController rake;
  extern StepperController conveyor;
  extern StepperController sifter;

  constexpr int disableConveyorPin   = 11;   
  constexpr int disableSifterPin     = 12;   
  constexpr int disableRakePin       = 13;   

  /* Individual Functions */
  void runConveyor();
  void runSifter();
  void runRake();
}

#endif

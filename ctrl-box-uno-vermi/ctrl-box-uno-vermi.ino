#include "servo.h"
#include "stepper.h"
#include "serial.h"

unsigned long baudRate = 115200;

void setup() {
  Serial.begin(baudRate);

  setupDiverter();

  stepper::conveyor.begin();
  stepper::sifter.begin();
  stepper::rake.begin();

  stepper::conveyor.name = "Conveyor";
  stepper::sifter.name = "Sifter";
  stepper::rake.name = "Rake";
  
  // pinMode(stepper::disableConveyorPin, OUTPUT);
  // pinMode(stepper::disableSifterPin, OUTPUT);
  // pinMode(stepper::disableRakePin, OUTPUT);

  // digitalWrite(stepper::disableConveyorPin, HIGH);
  // digitalWrite(stepper::disableSifterPin, HIGH);
  // digitalWrite(stepper::disableRakePin, HIGH);
  
  Serial.println("<info:[Uno] Board initialized>");
}

void loop() {
  recvWithStartEndMarkers();
  processSerialData();

  stepper::runConveyor();
  stepper::runSifter();
  stepper::runRake();
  
  updateDiverter();
  conveyorHaltTemp();
}
#include "servo.h"
#include "stepper.h"

bool newData = false;
const byte numChars = 100;
char receivedChars[numChars];

void recvWithStartEndMarkers() {
  static bool recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        receivedChars[ndx] = '\0';
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void processSerialData() {
  if (newData == true) {
    char commandChar = receivedChars[0];

    switch (commandChar) {
      case 'C':
        {
          char* firstCommand = strchr(receivedChars, ':');
          if (!firstCommand || strlen(firstCommand) < 2) {
            Serial.println("<error:Invalid sifter format>");
            break;
          }

          char actionChar = *(firstCommand + 1);
          int motorSpeed = 1000;

          char* secondCommand = strchr(firstCommand + 2, ':');
          if (secondCommand) {
            int parsed = atoi(secondCommand + 1);
            if (parsed > 0) {
              motorSpeed = parsed;
            }
          }

          switch (actionChar) {
            case 'A':
              {
                stepper::conveyor.receivedSpeed = motorSpeed;

                break;
              }

            case 'F':
              {
                stepper::conveyor.directionMultiplier = motorSpeed == 1 ? 1 : -1;

                break;
              }

            case 'C':
              {
                stepper::conveyor.returningToZero = false;

                stepper::conveyor.receivedSteps = 10000000;
                stepper::conveyor.receivedSpeed = motorSpeed;
                stepper::conveyor.directionMultiplier = 1;

                digitalWrite(stepper::disableConveyorPin, LOW);

                stepper::conveyor.isProcessing = true;
                stepper::conveyor.rotateRelative();

                Serial.println("<Conveyor:Active>");
                break;
              }

            case 'E':
              {
                char* secondCommand = strchr(firstCommand + 2, ':');

                if (secondCommand != NULL) {
                  char commandType = secondCommand[1];

                  if (commandType == 'B') {
                    if (diverterState == IDLE || diverterState == RETRACTING) {
                      setDiverterState(EXTENDING);
                    } else {
                      Serial.println("<warn:Diverter:Already Extended or In Progress>");
                    }

                  } else if (commandType == 'U') {
                    if (diverterState == HOLDING || diverterState == EXTENDING) {
                      setDiverterState(RETRACTING);
                    } else {
                      Serial.println("<warn:Diverter:Already Retracted or In Progress>");
                    }
                  }
                }

                break;
              }

            case 'V':
              {
                stepper::conveyor.returningToZero = false;

                stepper::conveyor.receivedSteps = 20000;
                stepper::conveyor.receivedSpeed = 2000;
                stepper::conveyor.directionMultiplier = -1;

                digitalWrite(stepper::disableConveyorPin, LOW);

                stepper::conveyor.isProcessing = true;
                stepper::conveyor.rotateRelative();

                Serial.println("<Conveyor:Active>");
                break;
              }

            case 'R':
              {
                stepper::conveyor.returningToZero = false;

                stepper::conveyor.isProcessing = true;
                stepper::conveyor.isActive = true;

                stepper::conveyor.returnOrigin();

                break;
              }

            case 'S':
              {
                stepper::conveyor.returningToZero = false;

                stepper::conveyor.isProcessing = false;
                stepper::conveyor.isActive = true;

                stepper::conveyor.stop();

                Serial.println("<Conveyor:Idle>");
                break;
              }

            default:
              Serial.println("<error:Unknown conveyor action>");
              break;
          }
          break;
        }

      case 'R':
        {
          char* firstCommand = strchr(receivedChars, ':');
          if (!firstCommand || strlen(firstCommand) < 2) {
            Serial.println("<error:Invalid rake format>");
            break;
          }

          char actionChar = *(firstCommand + 1);
          int cycleCount = 5;

          char* secondCommand = strchr(firstCommand + 2, ':');
          if (secondCommand) {
            int parsed = atoi(secondCommand + 1);
            if (parsed > 0) {
              cycleCount = parsed;
            }
          }

          switch (actionChar) {
            case 'P':
              {
                stepper::rake.returningToZero = false;

                stepper::rake.receivedSteps = 5000;
                stepper::rake.receivedSpeed = 1500;
                stepper::rake.directionMultiplier = -1;

                stepper::rake.totalCycles = cycleCount;
                stepper::rake.cycleCount = 0;

                stepper::rake.isProcessing = true;
                stepper::rake.rakeIsRunning = true;
                stepper::rake.rakeIsRunningCommand = true;

                stepper::rake.rotateAbsolute();

                Serial.print("<info:Rake:Active for ");
                Serial.print(stepper::rake.totalCycles);
                Serial.println(" cycles>");

                break;
              }

            case 'R':
              {
                stepper::rake.returningToZero = false;

                stepper::rake.isProcessing = true;
                stepper::rake.isActive = true;

                stepper::rake.returnOrigin();

                break;
              }

            case 'S':
              {
                stepper::rake.returningToZero = false;

                stepper::rake.isProcessing = false;
                stepper::rake.isActive = true;

                stepper::rake.stop();

                Serial.println("<info:Rake:Idle>");
                break;
              }

            default:
              Serial.println("<error:Unknown rake command>");
              break;
          }

          break;
        }

      case 'S':
        {
          char* firstCommand = strchr(receivedChars, ':');
          if (!firstCommand || strlen(firstCommand) < 2) {
            Serial.println("<error:Invalid sifter format>");
            break;
          }

          char actionChar = *(firstCommand + 1);
          int cycleCount = 5;

          char* secondCommand = strchr(firstCommand + 2, ':');
          if (secondCommand) {
            int parsed = atoi(secondCommand + 1);
            if (parsed > 0) {
              cycleCount = parsed;
            }
          }

          switch (actionChar) {
            case 'P':
              {
                stepper::sifter.returningToZero = false;

                stepper::sifter.receivedSteps = 5000;
                stepper::sifter.receivedSpeed = 1000;
                stepper::sifter.directionMultiplier = -1;

                stepper::sifter.totalCycles = cycleCount;
                stepper::sifter.cycleCount = 0;

                stepper::sifter.isProcessing = true;
                stepper::sifter.rakeIsRunning = true;
                stepper::sifter.rakeIsRunningCommand = true;

                stepper::sifter.rotateAbsolute();

                Serial.print("<info:Sifter:Active for ");
                Serial.print(stepper::sifter.totalCycles);
                Serial.println(" cycles>");

                break;
              }

            case 'R':
              {
                stepper::sifter.returningToZero = false;

                stepper::sifter.isProcessing = true;
                stepper::sifter.isActive = true;

                stepper::sifter.returnOrigin();

                break;
              }

            case 'S':
              {
                stepper::sifter.returningToZero = false;

                stepper::sifter.isProcessing = false;
                stepper::sifter.isActive = true;

                stepper::sifter.stop();

                Serial.println("<Sifter:Idle>");
                break;
              }

            default:
              Serial.println("<error:Unknown sifter command>");
              break;
          }

          break;
        }

      default:
        Serial.println("<error:Unknown command>");
        break;
    }

    newData = false;
  }
}
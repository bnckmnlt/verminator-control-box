#include "env.h"
#include "soil.h"
#include "relay.h"
#include "serial.h"
#include "aeration.h"
#include "sifter.h"
#include "scale.h"
#include "misting.h"
#include "vermitea.h"

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
      case 'A':
        {
          char *strtokIndx;

          strtokIndx = strtok(receivedChars, ":");

          strtokIndx = strtok(NULL, ":");
          bool aerationOn = (strtokIndx != NULL && atoi(strtokIndx) == 1);

          strtokIndx = strtok(NULL, ":");

          bool indefinite = false;
          unsigned long duration = 10;

          if (strtokIndx != NULL) {
            char firstChar = toupper(strtokIndx[0]);
            if (firstChar == 'I') {
              indefinite = true;
            } else {
              duration = atol(strtokIndx);
            }
          }

          aeration::setConfig(aerationOn, indefinite ? 0 : duration, indefinite);

          break;
        }

      case 'F':
        {
          char *strtokIndx;

          strtokIndx = strtok(receivedChars, ":");

          strtokIndx = strtok(NULL, ":");
          bool fanOn = (strtokIndx != NULL && atoi(strtokIndx) == 1);

          strtokIndx = strtok(NULL, ":");

          bool indefinite = false;
          unsigned long duration = 10;

          if (strtokIndx != NULL) {
            char firstChar = toupper(strtokIndx[0]);
            if (firstChar == 'I') {
              indefinite = true;
            } else {
              duration = atol(strtokIndx);
            }
          }

          env::setMode(fanOn, indefinite ? 0 : duration, indefinite);

          break;
        }

      case 'P':
        {
          char *strtokIndx;

          strtokIndx = strtok(receivedChars, ":");

          strtokIndx = strtok(NULL, ":");
          bool fanOn = (strtokIndx != NULL && atoi(strtokIndx) == 1);

          soil::setMode(fanOn);

          break;
        }

      case 'R':
        {
          int boardNumber = -1;
          int relayNumber = -1;
          int relayPower = -1;
          char *strtokIndx;
          char buff[50];

          strtokIndx = strtok(receivedChars, ":");
          strtokIndx = strtok(NULL, ":");
          if (strtokIndx != NULL) boardNumber = atoi(strtokIndx);

          strtokIndx = strtok(NULL, ":");
          if (strtokIndx != NULL) relayNumber = atoi(strtokIndx);

          strtokIndx = strtok(NULL, ":");
          if (strtokIndx != NULL) relayPower = atoi(strtokIndx);

          if (boardNumber >= 0 && relayNumber >= 0 && relayPower >= 0) {
            relay::triggerRelay(boardNumber, relayNumber, relayPower);
          } else {
            Serial.println("<error: Invalid relay command>");
          }
          break;
        }

      case 'M':
        {
          char *strtokIndx;

          strtokIndx = strtok(receivedChars, ":");

          strtokIndx = strtok(NULL, ":");
          bool mistingOn = (strtokIndx != NULL && atoi(strtokIndx) == 1);

          strtokIndx = strtok(NULL, ":");

          bool indefinite = false;
          unsigned long duration = 10;

          if (strtokIndx != NULL) {
            char firstChar = toupper(strtokIndx[0]);
            if (firstChar == 'I') {
              indefinite = true;
            } else {
              duration = atol(strtokIndx);
            }
          }

          misting::setConfig(mistingOn, indefinite ? 0 : duration, indefinite);

          break;
        }

        case 'V':
        {
          char *strtokIndx;

          strtokIndx = strtok(receivedChars, ":");

          strtokIndx = strtok(NULL, ":");
          bool vermiteaOn = (strtokIndx != NULL && atoi(strtokIndx) == 1);

          strtokIndx = strtok(NULL, ":");

          bool indefinite = false;
          unsigned long duration = 10;

          if (strtokIndx != NULL) {
            char firstChar = toupper(strtokIndx[0]);
            if (firstChar == 'I') {
              indefinite = true;
            } else {
              duration = atol(strtokIndx);
            }
          }

          vermitea::setConfig(vermiteaOn, indefinite ? 0 : duration, indefinite);

          break;
        }

      case 'H':
        {
          char *token = strtok(receivedChars, ":");
          token = strtok(NULL, ":");
          if (!token) break;

          scale::ScaleID calibrationId = scale::parseScaleID(token);

          token = strtok(NULL, ":");
          if (!token) break;

          if (strcmp(token, "BEGIN") == 0) {
            scale::calibrationId = calibrationId;
            scale::scaleCalMode = true;
          } else if (strcmp(token, "SAVE") == 0) {
            scale::saveValue = true;
          } else if (strcmp(token, "EXIT") == 0) {
            scale::resumeCalibration = false;
            scale::scaleCalMode = false;
          } else if (strcmp(token, "SETMASS") == 0) {
            token = strtok(NULL, ":");
            if (token) {
              scale::knownMass = atof(token);
              Serial.print("<info: HX711: Known mass set to ");
              Serial.print(scale::knownMass);
              Serial.println(" g>");
            }
          } else if (strcmp(token, "CONTINUE") == 0) {
            scale::proceed = true;
          }

          break;
        }


      default:
        Serial.println("<error: Unknown command>");
        break;
    }

    newData = false;
  }
}

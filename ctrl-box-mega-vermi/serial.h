#ifndef SERIAL_H
#define SERIAL_H

#include <Arduino.h>
#include <string.h>

extern bool newData;
extern const byte numChars;
extern char receivedChars[];

void recvWithStartEndMarkers();
void processSerialData();

#endif

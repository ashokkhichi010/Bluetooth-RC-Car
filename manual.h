#ifndef MANUAL_H
#define MANUAL_H

#include "motor_functions.h"

char manualCmd = 'S';

void handleManualCommand(char cmd) {
  switch (cmd) {
    case 'F':
    case 'B':
    case 'L':
    case 'R':
    case 'S':
    case 'G':
    case 'I':
    case 'H':
    case 'J':
      manualCmd = cmd;
      break;
  }
}

void resetManualCommand() {
  manualCmd = 'S';
}

void runManual() {
  switch (manualCmd) {
    case 'F': forward(); break;
    case 'B': backward(); break;
    case 'L': turnLeft(100); break;
    case 'R': turnRight(100); break;
    case 'S': stopCar(); break;

    case 'G': forwardLeft(); break;
    case 'I': forwardRight(); break;
    case 'H': backwardLeft(); break;
    case 'J': backwardRight(); break;
    // default: stopCar(); break;
  }
}

#endif

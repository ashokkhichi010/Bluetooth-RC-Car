#ifndef MANUAL_H
#define MANUAL_H

char manualCmd = "";

void handleManualCommand(char cmd) {
  manualCmd = cmd;
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
  }
}

#endif

#ifndef MANUAL_H
#define MANUAL_H

#include "motor_functions.h"
#include "logs.h"

MotionDirection manualMotion = MOTION_STOP;

void menualController(StreamData data) {
  String newDirection = data.stringData();
  logInfo("New Direction: " + newDirection);

  if (newDirection.equalsIgnoreCase("FORWARD")) {
    manualMotion = MOTION_FORWARD;
  } else if (newDirection.equalsIgnoreCase("BACKWARD")) {
    manualMotion = MOTION_BACKWARD;
  } else if (newDirection.equalsIgnoreCase("LEFT")) {
    manualMotion = MOTION_LEFT;
  } else if (newDirection.equalsIgnoreCase("RIGHT")) {
    manualMotion = MOTION_RIGHT;
  } else if (newDirection.equalsIgnoreCase("FORWARD_LEFT")) {
    manualMotion = MOTION_FORWARD_LEFT;
  } else if (newDirection.equalsIgnoreCase("FORWARD_RIGHT")) {
    manualMotion = MOTION_FORWARD_RIGHT;
  } else if (newDirection.equalsIgnoreCase("BACKWARD_LEFT")) {
    manualMotion = MOTION_BACKWARD_LEFT;
  } else if (newDirection.equalsIgnoreCase("BACKWARD_RIGHT")) {
    manualMotion = MOTION_BACKWARD_RIGHT;
  } else {
    manualMotion = MOTION_STOP;
  }

  logInfo("Manual direction updated from stream: " + newDirection);
}

void resetManualCommand() {
  manualMotion = MOTION_STOP;
}

void runManual() {
  applyMotionDirection(manualMotion);
}

#endif

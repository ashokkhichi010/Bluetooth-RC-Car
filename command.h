#ifndef COMMAND_H
#define COMMAND_H

#include <Arduino.h>

enum Mode {
  MANUAL_MODE,
  LINE_MODE,
  AUTO_MODE,
  FOLLOW_ME_MODE,
};

enum MotionDirection {
  MOTION_STOP,
  MOTION_FORWARD,
  MOTION_BACKWARD,
  MOTION_LEFT,
  MOTION_RIGHT,
  MOTION_FORWARD_LEFT,
  MOTION_FORWARD_RIGHT,
  MOTION_BACKWARD_LEFT,
  MOTION_BACKWARD_RIGHT,
};

const char *modeName(Mode mode) {
  switch (mode) {
    case MANUAL_MODE:
      return "MANUAL";
    case LINE_MODE:
      return "LINE";
    case AUTO_MODE:
      return "AUTO";
    case FOLLOW_ME_MODE:
      return "FOLLOW_ME";
  }

  return "MANUAL";
}

bool parseModeName(const String &value, Mode &mode) {
  if (value.equalsIgnoreCase("MANUAL")) {
    mode = MANUAL_MODE;
    return true;
  }
  if (value.equalsIgnoreCase("LINE")) {
    mode = LINE_MODE;
    return true;
  }
  if (value.equalsIgnoreCase("AUTO")) {
    mode = AUTO_MODE;
    return true;
  }
  if (value.equalsIgnoreCase("FOLLOW_ME") || value.equalsIgnoreCase("FOLLOWME")) {
    mode = FOLLOW_ME_MODE;
    return true;
  }

  return false;
}

#endif

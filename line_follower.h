#ifndef LINE_H
#define LINE_H

#include "pin_map.h"
#include "motor_functions.h"

#define LINE_LEFT LINE_LEFT_PIN
#define LINE_RIGHT LINE_RIGHT_PIN

void setupLine() {
  pinMode(LINE_LEFT, INPUT);
  pinMode(LINE_RIGHT, INPUT);
}

void runLineFollower() {
  int left = digitalRead(LINE_LEFT);
  int right = digitalRead(LINE_RIGHT);

  if (left == 0 && right == 0) {
    moveForward(150);
  } else if (left == 1 && right == 0) {
    turnLeft(150);
  } else if (left == 0 && right == 1) {
    turnRight(150);
  } else {
    stopCar();
  }
}

#endif
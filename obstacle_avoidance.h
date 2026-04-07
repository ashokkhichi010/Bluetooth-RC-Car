// version 1 with IR sensors
#ifndef OBS_H
#define OBS_H

#include "pin_map.h"

#define OBS_LEFT IR_OBSTACL_LEFT_PIN
#define OBS_RIGHT IR_OBSTACL_RIGHT_PIN

void setupObs() {
  pinMode(OBS_LEFT, INPUT);
  pinMode(OBS_RIGHT, INPUT);
}

void runObstacleAvoidance() {

  bool leftObs = (digitalRead(OBS_LEFT) == LOW);
  bool rightObs = (digitalRead(OBS_RIGHT) == LOW);

  if (!leftObs && !rightObs) {
    moveForward(150);
    // Serial.println("MODE:OBS;DIR:FWD;OBS:N;");
  } else if (leftObs && !rightObs) {
    turnRight(250);
    // Serial.println("MODE:OBS;DIR:RIGHT;OBS:L;");
  } else if (!leftObs && rightObs) {
    turnLeft(250);
    // Serial.println("MODE:OBS;DIR:LEFT;OBS:R;");
  } else {
    stopCar();
    // Serial.println("MODE:OBS;DIR:STOP;OBS:B;");
  }
}

#endif

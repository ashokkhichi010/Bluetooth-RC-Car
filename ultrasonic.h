#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>
#include "pin_map.h"
#include "firebase.h"

#define TRIG ULTRASONIC_TRIG_PIN
#define ECHO ULTRASONIC_ECHO_PIN

#define MAX_DISTANCE 300
#define ULTRASONIC_TIMEOUT_US 30000

void setupUltrasonic() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  digitalWrite(TRIG, LOW);
}

// -------- RAW DISTANCE --------
float getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, ULTRASONIC_TIMEOUT_US);

  if (duration == 0) return MAX_DISTANCE;

  float dist = duration * 0.0343 / 2.0;

  if (dist == 0) {
    return getDistance();
  }

  updateObstaclDistance(dist);
  return dist;
}

// -------- OBSTACLE CHECK --------
bool checkObstacleFound(int &distance, int minDist) {
  float dist = getDistance();
  distance = (int)dist;

  return dist <= minDist;
}

#endif

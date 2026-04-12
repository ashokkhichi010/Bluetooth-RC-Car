#ifndef OBS_MAIN_H
#define OBS_MAIN_H

#include "Arduino.h"
#include "ultrasonic.h"
#include "servo.h"
#include "motor_functions.h"

// --- Constants ---
const int BREAK_DIST = 50;
const int STOP_DIST = 10;
const int HIGHEST_SPEED = 180;
const int LOW_SPEED = 20;
const int OBSTACLE_CONFIRM_COUNT = 5;

// --- State machines ---
enum States {
  MOVE,
  SEARCH,
  TURN,
};

enum SearchStates {
  SCAN_LEFT,
  SCAN_RIGHT,
};

States currentState = MOVE;
SearchStates currentSearchState = SCAN_LEFT;

// --- Runtime vars ---
int targetSpeed = 0;
int obstacleCount = 0;

int centerDist = 0;
int stopDist = 0;
int leftDist = 0;
int rightDist = 0;

unsigned long lastSpeedUpdate = 0;

void setupObs() {
  setupUltrasonic();
  setupServoScanner();
  currentState = MOVE;
  currentSearchState = SCAN_LEFT;
  targetSpeed = 0;
  obstacleCount = 0;
}

void resetObstacleAvoidanceState() {
  currentState = MOVE;
  currentSearchState = SCAN_LEFT;
  targetSpeed = 0;
  obstacleCount = 0;
  centerDist = 0;
  stopDist = 0;
  leftDist = 0;
  rightDist = 0;
  lastSpeedUpdate = millis();
  centerServoNow();
  stopCar();
}

bool isObstacleTooClose() {
  return checkObstacleFound(stopDist, STOP_DIST);
}

void runObstacleAvoidance() {
  updateSpeed(targetSpeed);

  if (currentState != TURN) {
    forward();
  }

  switch (currentState) {
    case MOVE:
      if (checkObstacleFound(centerDist, BREAK_DIST)) {
        // updates centerDist as side effect
        obstacleCount++;
        if (obstacleCount >= OBSTACLE_CONFIRM_COUNT) {
          obstacleCount = 0;
          currentState = SEARCH;
          currentSearchState = SCAN_LEFT;
          targetSpeed = LOW_SPEED;
        }
      } else {
        targetSpeed = HIGHEST_SPEED;
        obstacleCount = 0;
      }
      break;
    case SEARCH:
      if (currentSearchState == SCAN_LEFT) {
        turnServoLeft();

        if (isSeroAtLeft()) {
          delay(100);  // Wait for servo to settle
          leftDist = getDistance();
          currentSearchState = SCAN_RIGHT;
        }
      } else if (currentSearchState == SCAN_RIGHT) {
        turnServoRight();

        if (isSeroAtRight()) {
          delay(100);  // Wait for servo to settle
          rightDist = getDistance();
          currentState = TURN;
        }
      }

      break;
    case TURN:
      setSpeedValue(4);
      if (leftDist < BREAK_DIST && rightDist < BREAK_DIST) {
        if (leftDist > rightDist) {
          backwardRight();
        } else {
          backwardLeft();
        }
      } else {
        if (leftDist > rightDist) {
          forwardLeft();
        } else {
          forwardRight();
        }
      }

      turnServoCenter();

      if (isSeroAtCenter()) {
        currentState = MOVE;
        targetSpeed = HIGHEST_SPEED;
        setSpeedValue(9);
      }
      break;
  }
}

#endif

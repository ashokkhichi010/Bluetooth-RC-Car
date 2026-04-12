#ifndef MOTOR_FUNCTIONS_H
#define MOTOR_FUNCTIONS_H

#include "command.h"
#include "pin_map.h"
#include "firebase.h"

// Motors (SAFE PWM)
#define ENA MOTOR_LEFT_PWM_PIN
#define ENB MOTOR_RIGHT_PWM_PIN

// Motor driver direction pins
#define IN1 MOTOR_LEFT_IN1_PIN
#define IN2 MOTOR_LEFT_IN2_PIN
#define IN3 MOTOR_RIGHT_IN1_PIN
#define IN4 MOTOR_RIGHT_IN2_PIN

int speedVal = 0;
MotionDirection currentMotionDirection = MOTION_STOP;

void stopCar();

void setSpeedValue(int newSpeed) {
  speedVal = constrain(newSpeed, 0, 255);
  updateCarSpeed(speedVal);
}

void updateSpeed(int targetSpeed) {
  if (speedVal == targetSpeed) return;

  if (targetSpeed == 0) {
    setSpeedValue(0);
  } else if (speedVal < targetSpeed) {
    setSpeedValue(speedVal + 1);
  } else if (speedVal > targetSpeed) {
    setSpeedValue(speedVal - 1);
  }
}

void speedController(StreamData data) {
  if (data.dataTypeEnum() != fb_esp_rtdb_data_type_integer && data.dataTypeEnum() != fb_esp_rtdb_data_type_float && data.dataTypeEnum() != fb_esp_rtdb_data_type_double) {
    logWarn("Speed stream ignored non-numeric payload");
    return;
  }

  speedVal = constrain(data.intData(), 0, 255);
  logInfo("Speed updated from stream: " + String(speedVal));
}

void setupMotors() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  stopCar();
}

void moveForward(int spd) {
  currentMotionDirection = MOTION_FORWARD;
  analogWrite(ENA, spd);
  analogWrite(ENB, spd);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward(int spd) {
  currentMotionDirection = MOTION_BACKWARD;
  analogWrite(ENA, spd);
  analogWrite(ENB, spd);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft(int spd) {
  currentMotionDirection = MOTION_LEFT;
  analogWrite(ENA, spd);
  analogWrite(ENB, spd);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight(int spd) {
  currentMotionDirection = MOTION_RIGHT;
  analogWrite(ENA, spd);
  analogWrite(ENB, spd);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopCar() {
  currentMotionDirection = MOTION_STOP;
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// -------- MOTOR FUNCTIONS --------
void forward() {
  moveForward(speedVal);
}

void backward() {
  moveBackward(speedVal);
}

void forwardLeft() {
  currentMotionDirection = MOTION_FORWARD_LEFT;
  analogWrite(ENA, speedVal / 2);
  analogWrite(ENB, speedVal);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void forwardRight() {
  currentMotionDirection = MOTION_FORWARD_RIGHT;
  analogWrite(ENA, speedVal);
  analogWrite(ENB, speedVal / 2);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backwardLeft() {
  currentMotionDirection = MOTION_BACKWARD_LEFT;
  analogWrite(ENA, speedVal / 2);
  analogWrite(ENB, speedVal);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void backwardRight() {
  currentMotionDirection = MOTION_BACKWARD_RIGHT;
  analogWrite(ENA, speedVal);
  analogWrite(ENB, speedVal / 2);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void setMotor(int left, int right) {
  currentMotionDirection = MOTION_FORWARD;
  analogWrite(ENA, constrain(left, 0, 255));
  analogWrite(ENB, constrain(right, 0, 255));

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void applyMotionDirection(MotionDirection direction) {
  switch (direction) {
    case MOTION_FORWARD:
      forward();
      break;
    case MOTION_BACKWARD:
      backward();
      break;
    case MOTION_LEFT:
      turnLeft(speedVal);
      break;
    case MOTION_RIGHT:
      turnRight(speedVal);
      break;
    case MOTION_FORWARD_LEFT:
      forwardLeft();
      break;
    case MOTION_FORWARD_RIGHT:
      forwardRight();
      break;
    case MOTION_BACKWARD_LEFT:
      backwardLeft();
      break;
    case MOTION_BACKWARD_RIGHT:
      backwardRight();
      break;
    case MOTION_STOP:
    default:
      stopCar();
      break;
  }
}

#endif

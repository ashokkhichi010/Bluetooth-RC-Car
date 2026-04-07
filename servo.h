#ifndef SERVO_SCAN_H
#define SERVO_SCAN_H

#include <Arduino.h>
#include <Servo.h>
#include "pin_map.h"

#define SERVO_PIN SERVO_SIGNAL_PIN
#define CENTER_ANGLE 90
#define LEFT_SCAN_ANGLE 170
#define RIGHT_SCAN_ANGLE 10

Servo scanServo;
int servoCurrentAngle = CENTER_ANGLE;

void setupServoScanner() {
  scanServo.attach(SERVO_PIN);
  scanServo.write(CENTER_ANGLE);
  servoCurrentAngle = CENTER_ANGLE;
}

void turnServoAt(int direction) {
  if (servoCurrentAngle < direction) {
    servoCurrentAngle = servoCurrentAngle + 2;
  } else {
    servoCurrentAngle = servoCurrentAngle - 2;
  }
  Serial.print(" servoCurrentAngle ");
  Serial.print(servoCurrentAngle);
  scanServo.write(servoCurrentAngle);
}

void centerServo() {
  turnServoAt(CENTER_ANGLE);
}

bool isServoAt(int angle) {
  return servoCurrentAngle == angle;
}

bool isSeroAtLeft() {
  return isServoAt(LEFT_SCAN_ANGLE);
}

bool isSeroAtRight() {
  return isServoAt(RIGHT_SCAN_ANGLE);
}

bool isSeroAtCenter() {
  return isServoAt(CENTER_ANGLE);
}

void setServoTargetAngle(int angle) {
  turnServoAt(angle);
}

void turnServoLeft() {
  turnServoAt(LEFT_SCAN_ANGLE);
}

void turnServoRight() {
  turnServoAt(RIGHT_SCAN_ANGLE);
}

void turnServoCenter() {
  turnServoAt(CENTER_ANGLE);
}

#endif

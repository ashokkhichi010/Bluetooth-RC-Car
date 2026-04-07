#ifndef UNO_PINS_H
#define UNO_PINS_H

#include <Arduino.h>

// Arduino Uno pin layout used by this project.
// Keep motor PWM on D5/D6 so Servo.h can use Timer1 without affecting drive PWM.
constexpr uint8_t BT_RX_PIN = 0;
constexpr uint8_t BT_TX_PIN = 1;

constexpr uint8_t MOTOR_LEFT_PWM_PIN = 5;
constexpr uint8_t MOTOR_RIGHT_PWM_PIN = 6;

constexpr uint8_t MOTOR_LEFT_IN1_PIN = 2;
constexpr uint8_t MOTOR_LEFT_IN2_PIN = 4;
constexpr uint8_t MOTOR_RIGHT_IN1_PIN = 7;
constexpr uint8_t MOTOR_RIGHT_IN2_PIN = 8;

constexpr uint8_t ULTRASONIC_TRIG_PIN = 12;
constexpr uint8_t ULTRASONIC_ECHO_PIN = 3;

constexpr uint8_t SERVO_SIGNAL_PIN = 9;

constexpr uint8_t LINE_LEFT_PIN = A0;
constexpr uint8_t LINE_RIGHT_PIN = A1;

constexpr uint8_t IR_OBSTACL_LEFT_PIN = A2;
constexpr uint8_t IR_OBSTACL_RIGHT_PIN = A3;

#endif

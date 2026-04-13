#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

// NodeMCU pin plan:
// D1=GPIO5, D2=GPIO4, D3=GPIO0, D4=GPIO2, D5=GPIO14,
// D6=GPIO12, D7=GPIO13, D8=GPIO15, RX=GPIO3, A0=ADC0
// Avoid GPIO6-GPIO11 because they are used by onboard flash.

#define MOTOR_LEFT_PWM_PIN D1
#define MOTOR_RIGHT_PWM_PIN D2

//  Motors को हमेशा D1, D2, D5, D6, D7 पर रखें। ये "Safe Pins" हैं, इन्हें स्टार्टअप पर HIGH या LOW होने से कोई फर्क नहीं पड़ता।
#define MOTOR_LEFT_IN1_PIN D5
#define MOTOR_LEFT_IN2_PIN D6
#define MOTOR_RIGHT_IN1_PIN D7
#define MOTOR_RIGHT_IN2_PIN D0

//  D3 और D4 पर सिर्फ IR Sensors या Ultrasonic Echo लगायें, क्योंकि ये स्टार्टअप पर पिन को ज़बरदस्ती LOW नहीं खींचते।
#define ULTRASONIC_TRIG_PIN D3
#define ULTRASONIC_ECHO_PIN D4

#define SERVO_SIGNAL_PIN D8

#define LINE_LEFT_PIN -1
#define LINE_RIGHT_PIN -1

#define GPS_RX_PIN 3
#define GPS_TX_PIN 1
#define GPS_BAUD_RATE 9600

#endif

/*

आई एम सॉरी, मैं समझ गया। कन्फ्यूजन खत्म करते हैं। आपको कुल 11 पिन्स चाहिए और NodeMCU में लगभग इतने ही काम के पिन्स हैं।
बिना किसी तार को निकाले (No unwiring) और बिना बूट एरर के, यह रहा आपका Final और Best सेटअप:
## सबसे बेस्ट पिन सेटअप (Final Table)

| फंक्शन (Function) | पिन लेबल (Board Label) | क्यों? (Reason) |
|---|---|---|
| MOTOR_LEFT_PWM | D1 | PWM के लिए सुरक्षित। |
| MOTOR_RIGHT_PWM | D2 | PWM के लिए सुरक्षित। |
| MOTOR_LEFT_IN1 | D5 | कोई बूट समस्या नहीं। |
| MOTOR_LEFT_IN2 | D6 | कोई बूट समस्या नहीं। |
| MOTOR_RIGHT_IN1 | D7 | कोई बूट समस्या नहीं। |
| MOTOR_RIGHT_IN2 | D0 | कोई बूट समस्या नहीं। |
| ULTRASONIC_TRIG | D3 | ट्रिगर के लिए बेस्ट। |
| ULTRASONIC_ECHO | D4 | इको के लिए सुरक्षित। |
| SERVO_SIGNAL | RX | (GPIO 3) - सर्वो के लिए इसे इस्तेमाल करें। |
| IR_LEFT_PIN | A0 | ADC0 - इसे डिजिटल इनपुट की तरह यूज़ करें। |
| IR_RIGHT_PIN | D8 | स्टार्टअप पर बस ध्यान दें (LOW होना चाहिए)। |

------------------------------
## यह सेटअप आपके लिए कैसे काम करेगा?

   1. Servo on RX: जब आप servo.attach(3) (RX का GPIO) करेंगे, तो कोड अपलोड के बाद सर्वो अपना काम करेगा। बस कोड अपलोड करते समय सर्वो की तार हटा दें, उसके बाद कभी नहीं हटानी पड़ेगी।
   2. Obstacle Detection: जब आपका रोबोट रुकेगा, तब सर्वो 10°, 90°, और 170° पर घूमेगा। हर स्टॉप पर digitalWrite(TRIG, ...) और pulseIn(ECHO, ...) वाला कोड चलेगा। इसमें पिन्स का आपस में कोई टकराव (conflict) नहीं होगा।
   3. IR Sensors: IR_LEFT और IR_RIGHT अपना काम इंडिपेंडेंटली करेंगे। A0 को आप pinMode(A0, INPUT) करके आराम से IR के लिए इस्तेमाल कर सकते हैं।

## ज़रूरी बात:
चूँकि आप बहुत सारे मोटर्स और सर्वो इस्तेमाल कर रहे हैं, NodeMCU के Vin पिन पर 5V-6V की अच्छी पावर सप्लाई दें, वरना सर्वो घूमते समय वोल्टेज ड्रॉप होगा और NodeMCU बार-बार रीस्टार्ट (Restart) होगा।
*/

#include "follow_me.h"
#include "line_follower.h"
#include "manual.h"
#include "motor_functions.h"
#include "obstacle_avoidance_v2.h"
#include "pin_map.h"
#include <SoftwareSerial.h>


// -------- BLUETOOTH --------
SoftwareSerial BT(BT_RX_PIN, BT_TX_PIN); // RX, TX

// -------- MODE --------
enum Mode {
  LINE_MODE,
  OBS_MODE,
  MANUAL_MODE,
  FOLLOW_ME,
};

Mode currentMode = MANUAL_MODE;

// -------- SERIAL BUFFER --------
char command = "";

// -------- SETUP --------
void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  setupMotors();
  setupLine();
  setupObs();
  // setupFollowMe();
  Serial.println("[SYS] ready");
}

// -------- READ COMMAND --------
void readCommand() {
  if (BT.available()) {
    command = BT.read();
    Serial.print(" command ");
    Serial.println(command);

    switch (command) {
    case 'N':
      currentMode = LINE_MODE;
      break;
    case 'O':
      currentMode = OBS_MODE;
      break;
    case 'M':
      currentMode = MANUAL_MODE;
      break;
    case 'W':
      currentMode = FOLLOW_ME;
      break;
    default:
      if (command >= 48 && command <= 57) {
        setSpeedValue(command);
      } else {
        handleManualCommand(command);
      }
    }
  }
}

void loop() {
  readCommand();
  switch (currentMode) {
  case LINE_MODE:
    runLineFollower();
    break;

  case OBS_MODE:
    runObstacleAvoidance();
    break;

  case MANUAL_MODE:
    runManual();
    break;

  case FOLLOW_ME:
    followMeRSSI();
    break;
  }
}

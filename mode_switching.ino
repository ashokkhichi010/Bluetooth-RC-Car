#include "follow_me.h"
#include "line_follower.h"
#include "manual.h"
#include "motor_functions.h"
#include "obstacle_avoidance_v2.h"
#include "pin_map.h"
#include <SoftwareSerial.h>


// -------- BLUETOOTH --------
SoftwareSerial softBT(BT_RX_PIN, BT_TX_PIN);  // RX, TX

// -------- MODE --------
enum Mode {
  LINE_MODE,
  OBS_MODE,
  MANUAL_MODE,
  FOLLOW_ME,
};

Mode currentMode = MANUAL_MODE;

// -------- SERIAL BUFFER --------
char command = '\0';

bool usesHardwareSerialForBluetooth() {
  return BT_RX_PIN == 0 && BT_TX_PIN == 1;
}

void beginBluetooth(unsigned long baudRate) {
  if (usesHardwareSerialForBluetooth()) {
    Serial.begin(baudRate);
  } else {
    softBT.begin(baudRate);
  }
}

int bluetoothAvailable() {
  return usesHardwareSerialForBluetooth() ? Serial.available() : softBT.available();
}

int bluetoothRead() {
  return usesHardwareSerialForBluetooth() ? Serial.read() : softBT.read();
}

void setMode(Mode newMode) {
  if (currentMode == newMode) {
    return;
  }

  stopCar();

  if (newMode == OBS_MODE) {
    resetObstacleAvoidanceState();
  } else if (newMode == MANUAL_MODE) {
    resetManualCommand();
  }

  currentMode = newMode;
}

// -------- SETUP --------
void setup() {
  //  Serial.begin(9600);
  beginBluetooth(9600);

  setupMotors();
  setupLine();
  setupObs();
  resetManualCommand();
  // setupFollowMe();
  Serial.println("[SYS] ready");
}

// -------- READ COMMAND --------
void readCommand() {
  while (bluetoothAvailable()) {
    command = (char)bluetoothRead();
    Serial.print(" command ");
    Serial.println(command);

    if (command == '\r' || command == '\n') {
      continue;
    }

    //  Serial.print(" command ");
    //  Serial.println(command);

    switch (command) {
      case 'N':
        setMode(LINE_MODE);
        break;
      case 'O':
        setMode(OBS_MODE);
        break;
      case 'M':
        setMode(MANUAL_MODE);
        break;
      case 'W':
        setMode(FOLLOW_ME);
        break;
      default:
        if (command >= '0' && command <= '9') {
          setSpeedValue(command - '0');
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

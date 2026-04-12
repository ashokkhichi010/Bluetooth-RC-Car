#include <math.h>

#include "command.h"
#include "firebase.h"
#include "follow_me.h"
#include "line_follower.h"
#include "manual.h"
#include "motor_functions.h"
#include "obstacle_avoidance.h"
#include "pin_map.h"

Mode currentMode = MANUAL_MODE;

unsigned long lastLocationUpdateAt = 0;
bool runtimeStreamsStarted = false;

void setMode(Mode newMode) {
  if (currentMode == newMode) {
    return;
  }

  stopCar();

  if (newMode == AUTO_MODE) {
    resetObstacleAvoidanceState();
  } else if (newMode == MANUAL_MODE) {
    resetManualCommand();
  }

  currentMode = newMode;
}

void updateModeLogic() {
  bool isObsTooClose = isObstacleTooClose();
  updateObstaclTooClose(isObsTooClose);
  
  if (isObsTooClose) {
    stopCar();
    resetManualCommand();
    return;
  }

  switch (currentMode) {
    case LINE_MODE:
      runLineFollower();
      break;
    case AUTO_MODE:
      runObstacleAvoidance();
      break;
    case MANUAL_MODE:
      runManual();
      break;
    case FOLLOW_ME_MODE:
      runFollowMe();
      break;
  }
}

void modeController(StreamData data) {
  String newMode = data.stringData();

  if (newMode.equalsIgnoreCase("LINE")) {
    setMode(LINE_MODE);
  } else if (newMode.equalsIgnoreCase("MANUAL")) {
    setMode(MANUAL_MODE);
  } else if (newMode.equalsIgnoreCase("AUTO")) {
    setMode(AUTO_MODE);
  } else if (newMode.equalsIgnoreCase("FOLLOW_ME") || newMode.equalsIgnoreCase("FOLLOWME")) {
    setMode(FOLLOW_ME_MODE);
  } else {
    logWarn("Unknown streamed mode: " + newMode);
    return;
  }

  logInfo("Mode updated from stream: " + newMode);
}

void setup() {
  Serial.begin(115200);
  logInfo("Booting robot firmware");
  setupWifiStorage();
  setupMotors();
  logInfo("Motors initialized");
  setupLine();
  logInfo("Line follower sensors initialized");
  setupObs();
  logInfo("Obstacle avoidance initialized");
  resetManualCommand();
  setupFollowMe();
  logInfo("Follow-me module initialized");
  startWifiConnectIfNeeded();
  setupFirebase();
  lastLocationUpdateAt = millis();
  logInfo("Setup complete. Waiting for WiFi before starting streams.");
}

void loop() {
  updateWiFi();

  if (!isWifiConnected()) {
    stopCar();
    runtimeStreamsStarted = false;
    streamsStarted = false;
    return;
  }

  if (!runtimeStreamsStarted) {
    startAllStreams(speedController, modeController, menualController);
    runtimeStreamsStarted = true;
  }

  updateHeartbeat();
  Firebase.ready();
  updateModeLogic();
}

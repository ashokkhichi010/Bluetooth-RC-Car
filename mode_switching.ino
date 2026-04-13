#include <math.h>

#include "command.h"
#include "logs.h"
#include "firebase.h"
#include "follow_me.h"
#include "line_follower.h"
#include "manual.h"
#include "motor_functions.h"
#include "obstacle_avoidance.h"
#include "pin_map.h"

Mode currentMode = MANUAL_MODE;

bool runtimeStreamsStarted = false;
bool followMeInitialized = false;
bool bootLogsCleared = false;

void clearLogs();
void carController(StreamData data);

void setMode(Mode newMode) {
  if (currentMode == newMode) {
    return;
  }

  stopCar();

  if (newMode == AUTO_MODE) {
    resetObstacleAvoidanceState();
  } else if (newMode == MANUAL_MODE) {
    resetManualCommand();
  } else if (newMode == FOLLOW_ME_MODE && !followMeInitialized) {
    setupFollowMe();
    followMeInitialized = true;
    logInfo("Follow-me module initialized");
  }

  if (newMode == FOLLOW_ME_MODE) {
    startDeviceLocationStreamIfNeeded(deviceLocationController);
  }

  currentMode = newMode;
}

void updateModeLogic() {
  bool isObsTooClose = isObstacleTooClose();
  updateObstaclTooClose(isObsTooClose);

  if (isObsTooClose) {
    backward();
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

void carController(StreamData data) {
  String path = data.dataPath();

  if (path.length() == 0 || path == "/") {
    if (data.dataTypeEnum() != fb_esp_rtdb_data_type_json) {
      return;
    }

    FirebaseJson *json = data.to<FirebaseJson *>();
    FirebaseJsonData jsonData;

    if (json != nullptr && json->get(jsonData, "speed")) {
      speedVal = constrain(jsonData.intValue, 0, 255);
      logInfo("Speed updated from car stream: " + String(speedVal));
    }

    if (json != nullptr && json->get(jsonData, "mode")) {
      String modeValue = jsonData.stringValue;
      Mode parsedMode;
      if (parseModeName(modeValue, parsedMode)) {
        setMode(parsedMode);
        logInfo("Mode updated from car stream: " + modeValue);
      }
    }

    if (json != nullptr && json->get(jsonData, "direction")) {
      String directionValue = jsonData.stringValue;
      setManualDirectionFromString(directionValue);
      logInfo("Manual direction updated from car stream: " + directionValue);
    }

    return;
  }

  if (path == "/speed") {
    speedController(data);
  } else if (path == "/mode") {
    modeController(data);
  } else if (path == "/direction") {
    menualController(data);
  }
}

void setup() {
  //  Serial.begin(115200);
  //  Serial.print("Start Connecting to Wi-Fi");
  startWifiConnectIfNeeded();
  //  Serial.println("Start Connecting to Firebase");
  setupFirebase();
  logInfo("Booting robot firmware");
  setupMotors();
  logInfo("Motors initialized");
  setupLine();
  logInfo("Line follower sensors initialized");
  setupObs();
  logInfo("Obstacle avoidance initialized");
  resetManualCommand();
  logInfo("Setup complete.");
}

void loop() {
  updateWiFi();
  Firebase.ready();

  if (!isWifiConnected()) {
    stopCar();
    runtimeStreamsStarted = false;
    streamsStarted = false;
    deviceLocationStreamStarted = false;
    bootLogsCleared = false;
    return;
  }

  if (!bootLogsCleared) {
    clearLogs();
    bootLogsCleared = true;
    logInfo("Firebase logs cleared for fresh boot.");
  }

  if (!runtimeStreamsStarted) {
    startAllStreams(carController);
    runtimeStreamsStarted = true;
  }

  updateHeartbeat();
  // updateGps();
  // publishGpsLocation();
  updateModeLogic();
}

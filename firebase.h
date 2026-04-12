#ifndef FIREBASE_H
#define FIREBASE_H

#include <FirebaseESP8266.h>
#include "command.h"
#include "wifi.h"
#include "paths.h"

FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;
FirebaseData firebaseData;
FirebaseJson firebaseJson;
FirebaseJsonData firebaseJsonData;

FirebaseData speedFB;
FirebaseData modeFB;
FirebaseData menualDirectionFB;

unsigned long lastFirebaseOnlineWriteAt = 0;
unsigned long lastHeartbeatAt = 0;
unsigned long lastCommandPollAt = 0;
bool lastOnlineStatus = false;
bool streamsStarted = false;

const unsigned long HEARTBEAT_INTERVAL_MS = 1000UL;
const unsigned long COMMAND_POLL_INTERVAL_MS = 250UL;

void setupFirebase() {
  logInfo("Initializing Firebase client");
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
  logInfo("Firebase initialized for host: " + String(FIREBASE_HOST));
}

bool firebaseSetBool(const String &path, bool value) {
  return Firebase.setBool(firebaseData, path, value);
}

bool firebaseSetInt(const String &path, int value) {
  return Firebase.setInt(firebaseData, path, value);
}

bool firebaseSetFloat(const String &path, float value) {
  return Firebase.setFloat(firebaseData, path, value);
}

bool firebaseSetString(const String &path, const String &value) {
  return Firebase.setString(firebaseData, path, value);
}

unsigned long currentTimestampSeconds() {
  return millis() / 1000UL;
}

void updateHeartbeat() {
  if (!isWifiConnected()) {
    return;
  }

  if (millis() - lastHeartbeatAt < HEARTBEAT_INTERVAL_MS) {
    return;
  }

  lastHeartbeatAt = millis();
  firebaseSetInt(LAST_SEEN_PATH, (int)currentTimestampSeconds());
}

void defaultStreamTimeoutCallback(bool timeout) {
  if (timeout) {
    logWarn("Firebase stream timed out, library will try to resume.");
  }
}

void startStreaming(String path, FirebaseData &dataObj, FirebaseData::StreamEventCallback callback, FirebaseData::StreamTimeoutCallback timeoutCallback = defaultStreamTimeoutCallback) {
  Firebase.setStreamCallback(dataObj, callback, timeoutCallback);

  if (!Firebase.beginStream(dataObj, path)) {
    logError("Could not start stream on " + path + ": " + dataObj.errorReason());
  } else {
    logInfo("Stream started on: " + path);
  }
}

void startSpeedStreaming(FirebaseData::StreamEventCallback callback) {
  startStreaming(SPEED_PATH, speedFB, callback);
}

void startModeStreaming(FirebaseData::StreamEventCallback callback) {
  startStreaming(MODE_PATH, modeFB, callback);
}

void startMenualDirectionStreaming(FirebaseData::StreamEventCallback callback) {
  startStreaming(DIRECTION_PATH, menualDirectionFB, callback);
}

void startAllStreams(FirebaseData::StreamEventCallback speedCallback, FirebaseData::StreamEventCallback modeCallback, FirebaseData::StreamEventCallback directionCallback) {
  if (streamsStarted || !isWifiConnected()) {
    return;
  }

  startSpeedStreaming(speedCallback);
  startModeStreaming(modeCallback);
  startMenualDirectionStreaming(directionCallback);
  streamsStarted = true;
  logInfo("All Firebase streams started.");
}

void updateServoAngle(int servoAngle) {
  firebaseSetInt(SERVO_ANGLE_PATH, servoAngle);
}

void updateCarLocation(int lat, int long) {}

void updateCarDirection(String direction) {
  firebaseSetString(DIRECTION_PATH, direction);
}

void updateObstaclDistance(float distance) {
  firebaseSetFloat(OBS_DISTANCE_PATH, distance);
}

void updateObstaclTooClose(bool isTooClose) {
  firebaseSetBool(OBS_TOO_CLOSE_PATH, isTooClose);
}

void updateCarSpeed(int speed) {
  firebaseSetInt(SPEED_PATH, speed);
}

void addLog(String log) {
  firebaseSetString(LOGS_PATH, log);
}

#endif

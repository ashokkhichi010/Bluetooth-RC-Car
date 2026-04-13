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

FirebaseData carFB;
FirebaseData deviceLocationFB;

unsigned long lastHeartbeatAt = 0;
bool streamsStarted = false;
bool deviceLocationStreamStarted = false;

const unsigned long HEARTBEAT_INTERVAL_MS = 1000UL;

bool firebaseCanWrite() {
  return isWifiConnected() && Firebase.ready();
}

void addLog(const char *log) {
  // 'setString' की जगह 'pushString' का उपयोग करें
  // 'pushString' हर बार एक नया unique ID बनाता है जिससे पुराने लॉग्स डिलीट नहीं होते
  // firebasePushString(LOGS_PATH, log);
  //  Serial.println(log);
  if (firebaseCanWrite()) {
    Firebase.pushString(firebaseData, LOGS_PATH, log);
  }
}

void setupFirebase() {
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
  logInfo("Firebase initialized for host: " + String(FIREBASE_HOST));
  //  Serial.println("Firebase initialized Successfully");
}

bool firebaseSetBool(const String &path, bool value) {
  if (!firebaseCanWrite()) {
    return false;
  }
  return Firebase.setBool(firebaseData, path, value);
}

bool firebaseSetInt(const String &path, int value) {
  if (!firebaseCanWrite()) {
    return false;
  }
  return Firebase.setInt(firebaseData, path, value);
}

bool firebaseSetFloat(const String &path, float value) {
  if (!firebaseCanWrite()) {
    return false;
  }
  return Firebase.setFloat(firebaseData, path, value);
}

bool firebaseSetString(const String &path, const String &value) {
  if (!firebaseCanWrite()) {
    return false;
  }
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

void startCarStreaming(FirebaseData::StreamEventCallback callback) {
  startStreaming(CAR_PATH, carFB, callback);
}

void startDeviceLocationStreaming(FirebaseData::StreamEventCallback callback) {
  startStreaming(DEVICE_LOCATION_PATH, deviceLocationFB, callback);
}

void startAllStreams(
  FirebaseData::StreamEventCallback carCallback) {
  if (streamsStarted || !isWifiConnected()) {
    return;
  }

  startCarStreaming(carCallback);
  streamsStarted = true;
  logInfo("All Firebase streams started.");
}

void startDeviceLocationStreamIfNeeded(FirebaseData::StreamEventCallback callback) {
  if (deviceLocationStreamStarted || !isWifiConnected()) {
    return;
  }

  startDeviceLocationStreaming(callback);
  deviceLocationStreamStarted = true;
  logInfo("Device location stream started.");
}

void updateServoAngle(int servoAngle) {
  firebaseSetInt(SERVO_ANGLE_PATH, servoAngle);
}

void updateCarLocation(float lat, float lng) {
  firebaseSetFloat(CAR_LOCATION_LAT_PATH, lat);
  firebaseSetFloat(CAR_LOCATION_LNG_PATH, lng);
}

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

void clearLogs() {
  if (!firebaseCanWrite()) {
    return;
  }
  Firebase.deleteNode(firebaseData, LOGS_PATH);
}

#endif

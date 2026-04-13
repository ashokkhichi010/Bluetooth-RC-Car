#ifndef FOLLOW_ME_H
#define FOLLOW_ME_H

#include <math.h>
#include "gps.h"
#include "motor_functions.h"

double targetLatitude = 0.0;
double targetLongitude = 0.0;
bool targetLocationValid = false;

double toRadians(double degrees) {
  return degrees * 0.017453292519943295;
}

double toDegrees(double radians) {
  return radians * 57.29577951308232;
}

double normalizeAngle(double angleDeg) {
  while (angleDeg > 180.0) {
    angleDeg -= 360.0;
  }
  while (angleDeg < -180.0) {
    angleDeg += 360.0;
  }
  return angleDeg;
}

double calculateDistanceMeters(double lat1, double lng1, double lat2, double lng2) {
  const double earthRadius = 6371000.0;
  double dLat = toRadians(lat2 - lat1);
  double dLng = toRadians(lng2 - lng1);
  double a = sin(dLat / 2.0) * sin(dLat / 2.0) +
             cos(toRadians(lat1)) * cos(toRadians(lat2)) *
             sin(dLng / 2.0) * sin(dLng / 2.0);
  double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
  return earthRadius * c;
}

double calculateBearingDeg(double lat1, double lng1, double lat2, double lng2) {
  double y = sin(toRadians(lng2 - lng1)) * cos(toRadians(lat2));
  double x = cos(toRadians(lat1)) * sin(toRadians(lat2)) -
             sin(toRadians(lat1)) * cos(toRadians(lat2)) * cos(toRadians(lng2 - lng1));
  double bearing = toDegrees(atan2(y, x));
  if (bearing < 0.0) {
    bearing += 360.0;
  }
  return bearing;
}

void deviceLocationController(StreamData data) {
  if (data.dataTypeEnum() == fb_esp_rtdb_data_type_json) {
    FirebaseJson *json = data.to<FirebaseJson *>();
    FirebaseJsonData jsonData;
    bool hasLat = false;
    bool hasLng = false;

    if (json != nullptr) {
      hasLat = json->get(jsonData, "lat") || json->get(jsonData, "latitude");
      if (hasLat) {
        targetLatitude = atof(jsonData.stringValue.c_str());
      }

      hasLng = json->get(jsonData, "lng") || json->get(jsonData, "longitude") || json->get(jsonData, "lon");
      if (hasLng) {
        targetLongitude = atof(jsonData.stringValue.c_str());
      }
    }

    targetLocationValid = hasLat && hasLng;
  } else if (data.dataTypeEnum() == fb_esp_rtdb_data_type_string) {
    String raw = data.stringData();
    int commaIndex = raw.indexOf(',');
    if (commaIndex > 0) {
      targetLatitude = raw.substring(0, commaIndex).toFloat();
      targetLongitude = raw.substring(commaIndex + 1).toFloat();
      targetLocationValid = true;
    }
  }

  if (targetLocationValid) {
    logInfo("Device location updated: lat=" + String(targetLatitude, 6) + ", lng=" + String(targetLongitude, 6));
  } else {
    logWarn("Device location stream payload could not be parsed");
  }
}

void setupFollowMe() {
  setupGps();
}

void runFollowMe() {
  updateGps();
  publishGpsLocation();

  if (!carGpsValid) {
    stopCar();
    firebaseSetString(FOLLOW_ME_STATUS_PATH, "waiting-for-gps");
    return;
  }

  if (!targetLocationValid) {
    stopCar();
    firebaseSetString(FOLLOW_ME_STATUS_PATH, "waiting-for-device-location");
    return;
  }

  double distanceMeters = calculateDistanceMeters(carLatitude, carLongitude, targetLatitude, targetLongitude);
  double targetBearing = calculateBearingDeg(carLatitude, carLongitude, targetLatitude, targetLongitude);
  double bearingError = normalizeAngle(targetBearing - carCourseDeg);

  firebaseSetFloat(FOLLOW_ME_DISTANCE_PATH, (float)distanceMeters);

  if (distanceMeters < 3.0) {
    stopCar();
    firebaseSetString(FOLLOW_ME_STATUS_PATH, "target-reached");
    return;
  }

  if (fabs(bearingError) > 45.0) {
    if (bearingError > 0.0) {
      turnRight(speedVal);
      firebaseSetString(FOLLOW_ME_STATUS_PATH, "turn-right");
    } else {
      turnLeft(speedVal);
      firebaseSetString(FOLLOW_ME_STATUS_PATH, "turn-left");
    }
  } else if (fabs(bearingError) > 15.0) {
    if (bearingError > 0.0) {
      forwardRight();
      firebaseSetString(FOLLOW_ME_STATUS_PATH, "adjust-right");
    } else {
      forwardLeft();
      firebaseSetString(FOLLOW_ME_STATUS_PATH, "adjust-left");
    }
  } else {
    forward();
    firebaseSetString(FOLLOW_ME_STATUS_PATH, "forward");
  }
}

#endif

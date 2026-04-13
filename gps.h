#ifndef GPS_H
#define GPS_H

#include <TinyGPS++.h>
#include "firebase.h"
#include "logs.h"
#include "paths.h"
#include "pin_map.h"

TinyGPSPlus gps;

double carLatitude = 0.0;
double carLongitude = 0.0;
double carCourseDeg = 0.0;
bool carGpsValid = false;
unsigned long lastGpsPublishAt = 0;
const unsigned long GPS_PUBLISH_INTERVAL_MS = 5000UL;
bool gpsSerialInitialized = false;

void setupGps() {
  if (gpsSerialInitialized) {
    return;
  }

//  Serial.begin(GPS_BAUD_RATE);
  gpsSerialInitialized = true;
  logInfo("GPS serial initialized on hardware RX/TX");
}

void updateGps() {
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  if (gps.location.isUpdated()) {
    carLatitude = gps.location.lat();
    carLongitude = gps.location.lng();
    carGpsValid = gps.location.isValid();

    if (gps.course.isValid()) {
      carCourseDeg = gps.course.deg();
    }
  }
}

void publishGpsLocation() {
  if (!isWifiConnected()) {
    return;
  }

  if (millis() - lastGpsPublishAt < GPS_PUBLISH_INTERVAL_MS) {
    return;
  }

  lastGpsPublishAt = millis();
  firebaseSetBool(CAR_GPS_VALID_PATH, carGpsValid);
  if (!carGpsValid) {
    logWarn("GPS location is not valid yet");
    return;
  }

  updateCarLocation((float)carLatitude,(float)carLongitude);
}

#endif

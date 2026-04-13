#ifndef WIFI_H
#define WIFI_H

#include <ESP8266WiFi.h>
#include "credentials.h"

const unsigned long WIFI_RETRY_INTERVAL_MS = 5000UL;

unsigned long lastWifiAttemptAt = 0;
bool wifiBeginIssued = false;
bool lastWifiConnectedState = false;

bool isWifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void startWifiConnectIfNeeded() {
  //  Serial.print(".");
  if (isWifiConnected()) {
    return;
  }

  if (wifiBeginIssued && millis() - lastWifiAttemptAt < WIFI_RETRY_INTERVAL_MS) {
    return;
  }

  lastWifiAttemptAt = millis();
  wifiBeginIssued = true;
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void updateWiFi() {
  if (isWifiConnected()) {
    if (!lastWifiConnectedState) {
      //  Serial.println("");
      //  Serial.println("Wi-Fi Connected Successfully");
    }
    lastWifiConnectedState = true;
    wifiBeginIssued = false;
    return;
  }

  if (lastWifiConnectedState) {
    lastWifiConnectedState = false;
  }

  if (millis() - lastWifiAttemptAt >= WIFI_RETRY_INTERVAL_MS) {
    startWifiConnectIfNeeded();
  }
}

#endif

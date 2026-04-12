#ifndef LOGS_H
#define LOGS_H

#include <Arduino.h> // String के लिए जरूरी

// 1. कंपाइलर को बताएं कि addLog नाम का फंक्शन कहीं और (firebase.h में) मौजूद है
void addLog(const char *log);

// लॉग के लिए बफर
extern char logBuffer[128]; 
char logBuffer[128]; 

void logInfo(const String &message) {
  snprintf(logBuffer, sizeof(logBuffer), "[INFO] %s", message.c_str());
  addLog(logBuffer); // अब एरर नहीं आएगा
}

void logWarn(const String &message) {
  snprintf(logBuffer, sizeof(logBuffer), "[WARN] %s", message.c_str());
  addLog(logBuffer);
}

void logError(const String &message) {
  snprintf(logBuffer, sizeof(logBuffer), "[ERROR] %s", message.c_str());
  addLog(logBuffer);
}

#endif

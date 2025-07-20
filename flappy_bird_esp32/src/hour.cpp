#include "hour.h"

namespace hour {

  bool alreadyInitialized = false;

  bool init() {

    unsigned long millisMarker = millis();

    Serial.printf("Starting connection to %s\n", SSID);
    WiFi.begin(SSID, PASSWORD);
    if (WiFi.status() != WL_CONNECTED) {
      Serial.printf(".");
      if(millis()-millisMarker > 30000) {
        Serial.println("\nFailed to connect");
        return false;
      }
    }
    Serial.printf("Connected to %s\n", SSID);
    alreadyInitialized = true;
    return true;
  }

  bool getLocalTime() {

    configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER);
    bool timeAvailable = getLocalTime(&timeInfo);

    if    (!timeAvailable) Serial.println("Failed to obtain time");
    else  {
      strftime(buffer, sizeof(buffer), TIME_FORMAT, &timeInfo);
      Serial.println(&timeInfo, TIME_FORMAT);
    }

    return timeAvailable;
  }

}


#include "hour.h"

namespace hour {

  std::tm timeInfo;
  bool alreadyInitialized = false;
  char hourBuffer[20];
  char dayBuffer[40];

  void init() {

    unsigned long millisMarker = millis();

    Serial.printf("Starting connection to %s\n", SSID);
    WiFi.begin(SSID, PASSWORD);
    if (WiFi.status() != WL_CONNECTED) {
      Serial.printf(".");
      if(millis()-millisMarker > 15000) {
        Serial.println("\nFailed to connect");
      }
    }
    Serial.printf("\nConnected to %s\n", SSID);
    alreadyInitialized = true;
  }

  bool getLocalTime() {

    configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER);
    bool timeAvailable = getLocalTime(&timeInfo);

    if    (!timeAvailable) Serial.println("Failed to obtain time");
    else  {
      sprintf(hourBuffer, "%02d:%02d:%02d",
        timeInfo.tm_hour,
        timeInfo.tm_min,
        timeInfo.tm_sec
      );
      sprintf(dayBuffer, "%s %02d %s",
        dias[timeInfo.tm_wday],
        timeInfo.tm_mday,
        meses[timeInfo.tm_mon]
      );
    }

    return timeAvailable;
  }

}


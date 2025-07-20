#include "hour.h"

namespace hour {

  std::tm timeInfo;
  bool alreadyInitialized = false;
  char hourBuffer[20];
  char dayBuffer[40];

  void init() {

    WiFi.disconnect(true);
    unsigned long millisMarker = millis();

    Serial.printf("Starting connection to %s\n", SSID);
    WiFi.begin(SSID, PASSWORD);
    delay(100);
    while (WiFi.status() != WL_CONNECTED) {
      if(millis()-millisMarker > 15000) {
        Serial.println("\nFailed to connect");
        return;
      }
    }
    Serial.printf("\nConnected to %s\n", SSID);
    configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER);
    delay(100);
    alreadyInitialized = true;

  }

  bool getLocalTime() {

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


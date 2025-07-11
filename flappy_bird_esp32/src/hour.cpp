#include "hour.h"

namespace hour {

  void init() {

    setlocale(LC_ALL, "es_ES.UTF-8");

    Serial.printf("Starting connection to %s\n", SSID);
    WiFi.begin(SSID, PASSWORD);
    connectedToWifi = false;

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

  void isAlreadyConnected() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.printf("Connecting to %s\n", SSID);
        return;
    }
    Serial.printf("Connected to %s\n", SSID);
    connectedToWifi = true;
  }

  void isWifiWorking () {
    if(WiFi.status() == WL_CONNECTED) connectionWorking = true;
    Serial.printf("Connection to %s is not working\n", SSID);
    connectionWorking = false;
  }

}


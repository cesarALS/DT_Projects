#include <WiFi.h>
#include "time.h"

const char* ssid       = "YOUR_SSID";       //name of your Wi-Fi network
const char* password   = "YOUR_PASS";       //password

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;         //Colombia: 5 hours behind Coordinated Universal Time (UTC): -5 x 60 x 60
const int   daylightOffset_sec = 0;         //no daylight offset

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void connectToWifi() {
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}
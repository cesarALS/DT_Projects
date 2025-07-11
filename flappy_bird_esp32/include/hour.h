#include <WiFi.h>
#include <ctime>
#include <locale.h>

#pragma once

namespace hour {

    const char* SSID       = "YOUR_SSID";       //name of your Wi-Fi network
    const char* PASSWORD   = "YOUR_PASS";       //password

    constexpr const char* NTP_SERVER        = "pool.ntp.org";
    constexpr long  GMT_OFFSET_SEC          = -18000;           //Colombia: 5 hours behind Coordinated Universal Time (UTC): -5 x 60 x 60
    constexpr int   DAY_LIGHT_OFFSET_SEC    = 0;                //no daylight offset
    constexpr const char* TIME_FORMAT       = "%A, %B %d %Y %H:%M:%S";

    extern bool connectedToWifi     = false;
    extern bool connectionWorking   = false;

    extern struct std::tm timeInfo;

    extern char buffer[80];

    void init();
    void isAlreadyConnected();
    void isWifiWorking();
    bool getLocalTime();
}
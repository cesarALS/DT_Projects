#include <WiFi.h>
#include <ctime>
#include <locale.h>

#pragma once

namespace hour {

    constexpr const char* SSID       = "SSID";       //name of your Wi-Fi network
    constexpr const char* PASSWORD   = "PASSWORD";       //password

    constexpr const char* NTP_SERVER        = "pool.ntp.org";
    constexpr long  GMT_OFFSET_SEC          = -18000;           //Colombia: 5 hours behind Coordinated Universal Time (UTC): -5 x 60 x 60
    constexpr int   DAY_LIGHT_OFFSET_SEC    = 0;                //no daylight offset
    constexpr const char* TIME_FORMAT       = "%A, %B %d %Y %H:%M:%S";

    extern struct std::tm timeInfo;

    constexpr const char* dias[] = {"Domingo", "Lunes", "Martes", "Mierco.", "Jueves", "Viernes", "Sabado"};
    constexpr const char* meses[] = {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiem.", "Octubre", "Noviem.", "Diciem."};

    extern bool alreadyInitialized;

    extern char hourBuffer[20];
    extern char dayBuffer[40];

    extern uint8_t timesTried;

    void init();
    bool getLocalTime();
}
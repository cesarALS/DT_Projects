#pragma once

#include "globals.h"

namespace menus {

    enum class Name {
        Init,
        FlappyBird,
        Hour,
        Portrait,
        Sensor
    };

    extern Name currentMode;
    extern std::unordered_map<Name, void(*)()> list;
    extern bool firstEntrance;

    void advance();
    void changeMenu(Name mode, int color = TFT_BLACK);

    void initMenu();
    void mainMenu();
    void gameMenu();
    void hourMenu();

}
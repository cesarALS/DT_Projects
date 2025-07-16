#pragma once

#include "globals.h"
#include "images.h"

namespace menus {

    enum class Name {
        Init,
        MainMenu,
        FlappyBird,
        Hour,
        Portrait,
        Sensor
    };

    extern Name currentMode;
    extern std::unordered_map<Name, void(*)()> list;
    extern bool firstEntrance;

    void advance();

    namespace InitMenu{
        extern TFT_eSprite panda;
        void menu();
    }

    namespace MainMenu{
        extern TFT_eSprite bgSpr;
        extern TFT_eSprite logSpr;
        extern assets::MainMenuLogo currentLogo;
        extern assets::logoId currentMenu;

        constexpr int arrowTriangleStartingX = 185;
        constexpr int arrowTriangleHalf = -20;
        constexpr int arrowTriangleBase = 20;

        void changeLogo();

        void menu();
    };

    void changeMenu(Name mode, int color = TFT_BLACK);

    void gameMenu();
    void hourMenu();

}
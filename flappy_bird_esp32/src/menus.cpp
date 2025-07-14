#include "menus.h"
#include "flappy_bird.h"

namespace menus {

    Name currentMode = Name::Init;
    std::unordered_map<Name, void(*)()> list;
    bool firstEntrance = true;

    void advance() {
        for (auto &mode : list) {
            if(currentMode == mode.first) {
                mode.second();
                return;
            }
        }
    }

    void changeMenu(Name mode) {
        currentMode = mode;
        firstEntrance = true;
    }

    void initMenu() {

        TFT_eSprite panda = TFT_eSprite(&tft);


        if (firstEntrance) {
            tft.fillScreen(TFT_WHITE);
            tft.setTextColor(TFT_BLACK);

            screen::initializeSprite(panda, PANDA_WIDTH, PANDA_HEIGHT, false, 16);
            panda.fillSprite(TFT_WHITE);
            tft.drawCentreString("Panda Voyager", tft.width()/2, tft.height()*0.1, 4);
            panda.pushImage(0, 0, PANDA_WIDTH, PANDA_HEIGHT, Panda);
            panda.pushSprite(tft.width()/2-panda.width()/2, tft.height()/2-panda.height()/2);

            std::array<TFT_eSprite*, 2> buttonsPair = screen::displayButtonIndications(tft, "Game", "Menu");

            firstEntrance = false;
        }

        if(button::list.at(button::RIGHT_ID)->consumeClick()) {
            changeMenu(Name::FlappyBird);
            panda.deleteSprite();
        }

    }

    void gameMenu() {

        if (firstEntrance) {
            tft.fillScreen(TFT_BLACK);
            game::canvas::changeBgStyle();
            firstEntrance = false;
        }

        game::advance();
    }




}
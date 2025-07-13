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

        if (firstEntrance) {
            tft.fillScreen(TFT_BLACK);
            tft.drawCentreString("Panda Voyager", tft.width()/2, tft.height()/2-10, 4);
            firstEntrance = false;
        }

        if(button::list.at(button::LEFT_ID)->consumeClick()) {
            changeMenu(Name::FlappyBird);
        }

    }

    void gameMenu() {

        if (firstEntrance) {
            tft.fillScreen(TFT_GREENYELLOW);
            firstEntrance = false;
        }

        game::advance();
    }




}
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

    void changeMenu(Name mode, int color) {
        currentMode = mode;
        screen::doubleWipe(5, color);
        firstEntrance = true;
    }

    void initMenu() {

        TFT_eSprite panda = TFT_eSprite(&tft);

        if (firstEntrance) {
            tft.fillScreen(TFT_WHITE);
            tft.setTextColor(TFT_BLACK);

            screen::initializeSprite(panda, assets::PANDA_WIDTH, assets::PANDA_HEIGHT, false, 16);
            panda.fillSprite(TFT_WHITE);
            tft.drawCentreString("Panda Voyager", tft.width()/2, tft.height()*0.1, 4);
            panda.pushImage(0, 0, assets::PANDA_WIDTH, assets::PANDA_HEIGHT, assets::Panda);
            panda.pushSprite(tft.width()/2-panda.width()/2, tft.height()/2-panda.height()/2);

            screen::displayButtonIndications(tft, "Hora", "Juego");

            firstEntrance = false;
        }

        if(button::list.at(button::RIGHT_ID)->consumeClick()) {
            panda.deleteSprite();
            changeMenu(Name::FlappyBird);
        }
        else if (button::list.at(button::LEFT_ID)->consumeClick()) {
            panda.deleteSprite();
            changeMenu(Name::Hour);
        }

    }

    void gameMenu() {

        if (firstEntrance) {
            tft.fillScreen(TFT_BLACK);
            game::init();
            firstEntrance = false;
        }

        if (game::state::current == game::state::opt::Menu) {

            game::canvas::gameMenuDisplay();

            if (button::list.at(game::PLAY_BUTTON)->consumeClick()) {
                game::state::reset();
                screen::doubleWipe(5, TFT_BLACK);
                game::state::current = game::state::opt::Playing;
            }

            else if (button::list.at(button::RIGHT_ID)->consumeClick()) {
                game::state::reset();
                game::canvas::destroySprites();
                changeMenu(Name::Init, TFT_WHITE);
            }
        }

        else if (game::state::current == game::state::opt::Playing) {

            game::state::advanceGame();

            if (button::list.at(button::RIGHT_ID)->consumeClick()) {
                game::state::current = game::state::Menu;
                screen::wipe(10, TFT_BLACK);
                button::reset();
            }

        }

    }

    void hourMenu() {
        if (firstEntrance) {
            tft.fillScreen(0x196b);
            screen::displayButtonIndications(tft, "", "Menu");

            tft.setTextColor(TFT_WHITE, TFT_WHITE);
            tft.drawCentreString("Hora Colombia", tft.width()*0.5, 20, 4);

            tft.drawCentreString("15:02:30", tft.width()*0.475, tft.height()*0.3, 7);
            tft.drawCentreString("Lunes 24 Julio", tft.width()*0.5, tft.height()*0.6, 4);

            firstEntrance = false;
        }

        if (button::list.at(button::RIGHT_ID)->consumeClick()) {
            changeMenu(Name::Init);
        }
    }




}
#include "menus.h"
#include "flappy_bird.h"
#include "hour.h"

namespace menus {

    Name currentMode = Name::Init;
    std::unordered_map<Name, void(*)()> list;
    bool firstEntrance = true;

    void init() {
        menus::list.emplace(menus::Name::Init, menus::InitMenu::menu);
        menus::list.emplace(menus::Name::MainMenu, menus::MainMenu::menu);
        menus::list.emplace(menus::Name::FlappyBird, menus::gameMenu);
        menus::list.emplace(menus::Name::Hour, menus::hourMenu);
        menus::list.emplace(menus::Name::Portrait, menus::portraitMenu);
    }

    void advance() {
        for (auto &mode : list) {
            if(currentMode == mode.first) {
                mode.second();
                return;
            }
        }
    }

    namespace InitMenu {
        TFT_eSprite panda = TFT_eSprite(&tft);

        void menu() {

            if (firstEntrance) {
                tft.fillScreen(TFT_WHITE);
                tft.setTextColor(TFT_BLACK);

                screen::initializeSprite(panda, assets::PANDA_WIDTH, assets::PANDA_HEIGHT, false, 16);
                panda.fillSprite(TFT_WHITE);
                tft.drawCentreString("Panda Voyager", tft.width()/2, tft.height()*0.1, 4);
                panda.pushImage(0, 0, assets::PANDA_WIDTH, assets::PANDA_HEIGHT, assets::Panda);
                panda.pushSprite(tft.width()/2-panda.width()/2, tft.height()/2-panda.height()/2);

                screen::displayButtonIndications(tft, "Juego", "Menu");

                firstEntrance = false;
            }

            if(button::list.at(button::RIGHT_ID)->consumeClick()) {
                panda.deleteSprite();
                changeMenu(Name::MainMenu);
            }
            else if (button::list.at(button::LEFT_ID)->consumeClick()) {
                panda.deleteSprite();
                changeMenu(Name::FlappyBird);
            }
        }
    }

    namespace MainMenu {

        TFT_eSprite logoSpr = TFT_eSprite(&screen::bgSpr);
        assets::logoId currentLogoId = assets::logoId::resizedBird;
        assets::MainMenuLogo currentLogo = assets::getMainMenuLogo(currentLogoId);

        void changeLogo() {
            currentLogoId = static_cast<assets::logoId>((static_cast<int>(currentLogoId) + 1) % assets::numLogoMenus);
            currentLogo = assets::getMainMenuLogo(currentLogoId);
        }

        void menu() {

            if(firstEntrance) {
                screen::initializeSprite(screen::bgSpr, 236, 236, true);
                screen::initializeSprite(logoSpr, assets::maxLogoDimensions, assets::maxLogoDimensions, false, 16);
                firstEntrance = false;
            }

            screen::bgSpr.fillScreen(0x196b);
            screen::bgSpr.setTextColor(TFT_WHITE, TFT_WHITE);
            screen::bgSpr.drawCentreString("Menu Principal", screen::bgSpr.width()/2, 10, 4);
            screen::displayButtonIndications(screen::bgSpr, "OK", "Cambiar");

            logoSpr.fillSprite(TFT_BLACK);

            logoSpr.pushImage(
                logoSpr.width()/2-currentLogo.width/2,
                logoSpr.height()/2-currentLogo.height/2,
                currentLogo.width,
                currentLogo.height,
                currentLogo.logo
            );

            logoSpr.pushToSprite(&screen::bgSpr, screen::bgSpr.width()*0.425-logoSpr.width()/2, screen::bgSpr.width()*0.175, TFT_BLACK);

            screen::bgSpr.fillTriangle(
                arrowTriangleStartingX, screen::bgSpr.height()/2+arrowTriangleHalf,
                arrowTriangleStartingX, screen::bgSpr.height()/2+arrowTriangleHalf+arrowTriangleBase,
                arrowTriangleStartingX+arrowTriangleBase, screen::bgSpr.height()/2+arrowTriangleHalf+arrowTriangleBase/2, TFT_GREEN
            );

            screen::bgSpr.pushSprite(1, 1);

            if(button::list.at(button::RIGHT_ID)->consumeClick()) {
                changeLogo();
            }
            else if(button::list.at(button::LEFT_ID)->consumeClick()) {
                switch (currentLogoId)
                {
                case (assets::logoId::resizedBird):
                    changeMenu(Name::FlappyBird, TFT_BLACK);
                    break;
                case (assets::logoId::hour):
                    changeMenu(Name::Hour, TFT_BLACK);
                    break;
                case (assets::logoId::portrait):
                    changeMenu(Name::Portrait, TFT_BLACK);
                    break;
                default:
                    changeMenu(Name::FlappyBird, TFT_BLACK);
                    break;
                }
                screen::bgSpr.deleteSprite();
                logoSpr.deleteSprite();
            }

        }
    }

    void changeMenu(Name mode, int color) {
        currentMode = mode;
        screen::doubleWipe(5, color);
        firstEntrance = true;
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
                changeMenu(Name::MainMenu, TFT_BLACK);
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
            tft.drawCentreString("Mierco. 26 Julio", tft.width()*0.5, tft.height()*0.6, 4);

            firstEntrance = false;
        }

        if (button::list.at(button::RIGHT_ID)->consumeClick()) {
            changeMenu(Name::MainMenu);
        }
    }

    void portraitMenu() {
        if(firstEntrance) {
            screen::initializeSprite(screen::bgSpr, screen::CANVAS_WIDTH, screen::CANVAS_HEIGHT, true);
        }

        screen::bgSpr.pushImage(
            screen::bgSpr.width()/2-assets::port::ALL_WIDTH/2,
            screen::bgSpr.height()*0.8/2-assets::port::ALL_HEIGHT/2,
            assets::port::ALL_WIDTH,
            assets::port::ALL_HEIGHT,
            assets::port::all
        );

        screen::displayButtonIndications(screen::bgSpr, "", "Menu");

        screen::bgSpr.pushSprite(screen::PADDING, screen::PADDING);

        if (button::list.at(button::RIGHT_ID)->consumeClick()) {
            changeMenu(Name::MainMenu);
            screen::bgSpr.deleteSprite();
        }
    }




}
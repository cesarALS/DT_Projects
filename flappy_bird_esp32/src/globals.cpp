#include "globals.h"

#define RANDOM_SEED_PIN 36

TFT_eSPI tft = TFT_eSPI();

void globalsInit() {

    tft.init();
    tft.setRotation(0);
    tft.setSwapBytes(true);

    randomSeed(analogRead(RANDOM_SEED_PIN));
}

namespace button {

    void Button::handlePress() {
        this->press = true;
    }

    void Button::handleClick() {
        this->pressAlreadyStarted = false;
        this->hasClicked = true;
    }

    void Button::pressWrapper(void* context) {
        static_cast<Button*>(context)->handlePress();
    }

    void Button::clickWrapper(void* context) {
        static_cast<Button*>(context)->handleClick();
    }

    Button::Button(uint8_t pin) :
        pin(pin),
        ob(OneButton(pin, true)),
        press(false),
        pressAlreadyStarted(false),
        hasClicked(false)
    {
        ob.attachPress(pressWrapper, (void*)this);
        ob.attachClick(clickWrapper, (void*)this);
        ob.setClickMs(200);
        ob.setDebounceMs(15);
    };

    bool Button::consumePress() {
        ob.tick();
        bool initialPress = false;
        if (press && !pressAlreadyStarted) {
            pressAlreadyStarted = true;
            initialPress = true;
        }
        this->press = false;
        return initialPress;
    };

    bool Button::consumeClick() {
        ob.tick();
        if(this->hasClicked) {
            this->hasClicked = false;
            return true;
        } else return false;
    }

    void Button::reset(){
        this->hasClicked = false;
        this->press = false;
        this->pressAlreadyStarted = false;
    }

    std::unordered_map<std::string, Button*> list;

    void init() {
        list[LEFT_ID] = new Button(LEFT);
        list[RIGHT_ID] = new Button(RIGHT);
    }

    void update() {
        for (auto &btn : list) {
            btn.second->ob.tick();
        }
    }

    void reset() {
        for (auto &btn : list) {
            btn.second->reset();
        }
    }
}

namespace screen {

    void initializeSprite(TFT_eSprite& spr, int w, int h, bool swap, uint8_t colorDepth) {
        spr.createSprite(w, h);
        spr.setColorDepth(colorDepth);
        spr.setSwapBytes(swap);
    }

    std::array<TFT_eSprite*, 2> displayButtonIndications(TFT_eSPI& spr, const char* leftText, const char* rightText) {

        TFT_eSprite* leftButton = new TFT_eSprite(&spr);
        TFT_eSprite* rightButton = new TFT_eSprite(&spr);

        std::array<TFT_eSprite*, 2> buttonsPair = {leftButton, rightButton};

        uint8_t i = 0;
        for (auto &btn : buttonsPair) {

            initializeSprite(*btn, spr.width()/2, spr.height()*0.2, false);

            btn->fillSprite(TFT_LIGHTGREY);
            btn->setTextColor(TFT_BLACK, TFT_BLACK);
            btn->drawCentreString(i==0 ? leftText : rightText, btn->width()/2, btn->height()*0.3, 4);

            btn->pushSprite(i==0 ? 0 : spr.width()/2, 0.8*spr.height());

            i++;

        }

        return buttonsPair;
    }

    void wipe(int speed, int color) {
        for (int i = 0; i < tft.height(); i += speed) {
            tft.fillRect(0, 0, tft.width(), i, color);
        }
    }

    void doubleWipe(int speed, int color) {
        for (int i = 0; i < tft.height()/2+speed; i += speed) {
            tft.fillRect(0, 0, tft.width(), i, color);
            tft.fillRect(0, tft.height()-i, tft.width(), tft.height()-i, color);
        }
    }

    void animateTextTopCenter(int speed, const char* text, int color) {
        doubleWipe(speed, color);
        tft.setTextColor(TFT_BLACK);
        tft.drawCentreString(text, tft.width()/2, tft.height()/2-10, 4);
    }
}
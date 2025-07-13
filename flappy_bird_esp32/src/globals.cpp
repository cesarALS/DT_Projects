#include "globals.h"

TFT_eSPI tft = TFT_eSPI();

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

    void reset() {
        for (auto &btn : list) {
            btn.second->reset();
        }
    }
}


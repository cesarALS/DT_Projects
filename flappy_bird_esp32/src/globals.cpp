#include "globals.h"

TFT_eSPI tft = TFT_eSPI();

namespace button {

    void Button::handle() {
        this->hasClicked = true;
    }

    void Button::handleWrapper(void* context) {
        static_cast<Button*>(context)->handle();
    }

    Button::Button(uint8_t pin) : pin(pin), ob(OneButton(pin, true)), hasClicked(false) {
        ob.attachClick(handleWrapper, (void*)this);
    };

    bool Button::consume() {
        ob.tick();
        if(this->hasClicked) {
            this->hasClicked = false;
            return true;
        } else return false;
    }

    std::unordered_map<std::string, Button*> list;

    void init() {
        list[LEFT_ID] = new Button(LEFT);
        list[RIGHT_ID] = new Button(RIGHT);
    }
}


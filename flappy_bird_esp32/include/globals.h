#pragma once
#include <TFT_eSPI.h>
#include "OneButton.h"
#include <unordered_map>
#include <string>

namespace button {

    constexpr uint8_t LEFT  = 13;
    constexpr uint8_t RIGHT = 14;

    constexpr const char* LEFT_ID = "left";
    constexpr const char* RIGHT_ID = "right";

    class Button {
        private:
            bool press;
            bool pressAlreadyStarted;
            bool hasClicked;
            uint8_t pin;

            void handlePress();
            void handleClick();
            static void pressWrapper(void* context);
            static void clickWrapper(void* context);
        public:
            OneButton ob;
            Button(uint8_t pin);
            bool consumePress();
            bool consumeClick();
            void reset();
    };

    extern std::unordered_map<std::string, Button*> list;

    void init();
    void update();
    void reset();
}

extern TFT_eSPI tft;

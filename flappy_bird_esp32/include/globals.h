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
            OneButton ob;
            bool hasClicked;
            uint8_t pin;

            void handle();
            static void handleWrapper(void* context);
        public:
            Button(uint8_t pin);
            bool consume();
    };

    extern std::unordered_map<std::string, Button*> list;

    void init();
}

extern TFT_eSPI tft;

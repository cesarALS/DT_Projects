#pragma once
#include <TFT_eSPI.h>
#include "OneButton.h"
#include <unordered_map>
#include <string>
#include <array>

extern TFT_eSPI tft;
void globalsInit();

namespace button {

    constexpr uint8_t LEFT  = 32;
    constexpr uint8_t RIGHT = 33;

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

namespace screen {

    constexpr uint8_t COLOR_DEPTH       = 8;

    void initializeSprite(TFT_eSprite& spr, int w, int h, bool swap, uint8_t colorDepth=COLOR_DEPTH);

    std::array<TFT_eSprite*, 2> displayButtonIndications(TFT_eSPI& spr, const char* leftText="", const char* rightText="");
    void wipe(int speed, int color);
    void doubleWipe(int speed, int color);
    void animateTextTopCenter(int speed, const char* text, int color);
}

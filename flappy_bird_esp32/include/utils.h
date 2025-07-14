#pragma once
#include <TFT_eSPI.h>
#include <array>

namespace screen {

    constexpr uint8_t COLOR_DEPTH       = 8;

    void initializeSprite(TFT_eSprite& spr, int w, int h, bool swap, uint8_t colorDepth=COLOR_DEPTH);

    std::array<TFT_eSprite*, 2> displayButtonIndications(TFT_eSPI& spr, const char* leftText="", const char* rightText="");
    void wipe(int speed, int color);
    void doubleWipe(int speed, int color);
    void animateTextTopCenter(int speed, const char* text, int color);
}


#pragma once
#include <TFT_eSPI.h>

namespace screen {
    void wipe(int speed, int color);
    void doubleWipe(int speed, int color);
    void animateTextTopCenter(int speed, const char* text, int color);
}


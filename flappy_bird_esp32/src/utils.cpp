#include "utils.h"
#include "globals.h"

namespace screen {

    void initializeSprite(TFT_eSprite& spr, int w, int h, bool swap, uint8_t colorDepth) {
        spr.createSprite(w, h);
        spr.setColorDepth(colorDepth);
        spr.setSwapBytes(swap);
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
#include "utils.h"
#include "globals.h"

void screenWipe(int speed) {
    for (int i = 0; i < tft.height(); i += speed) {
        tft.fillRect(0, 0, tft.width(), i, TFT_WHITE);
    }
}

void animateTextTopCenter(int speed, const char* text) {
    screenWipe(speed);
    tft.setTextColor(TFT_BLACK);
    tft.drawCentreString(text, tft.width()/2, tft.height()/2-10, 4);
}
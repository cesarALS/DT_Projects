#pragma once
#include <TFT_eSPI.h>

void screenWipe(int speed, int color);
void doubleWipe(int speed, int color);
void animateTextTopCenter(int speed, const char* text, int color);
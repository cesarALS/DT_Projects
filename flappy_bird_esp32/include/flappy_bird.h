#pragma once

#include "globals.h"
#include "images.h"
#include "math.h"

namespace game {

    constexpr const char* PLAY_BUTTON = button::LEFT_ID;

    void init();

    namespace canvas {

        constexpr uint8_t SATELITE_RADIUS = 25;

        struct colorPalette {
            int sky;
            int text;
            int wall;
            int satelite;
        };

        const colorPalette day = {
            0x66ff, TFT_BLACK, 0x6e84, 0xff67
        };
        const colorPalette night = {
            0x196b, TFT_WHITE, 0x5da2, 0xffff
        };

        extern const colorPalette* currentTime;

        void draw();
        void init();
        void staticRender();
        void changeBgStyle();
        void gameMenuDisplay();
        void destroySprites();

    };

    namespace walls {

        constexpr uint8_t NUM = 2;

        constexpr uint8_t WIDTH             = 30;
        constexpr uint8_t GAP               = 95;
        constexpr uint8_t MINIMAL_HEIGHT    = 20;                   // The minimum height of a semi-wall
        constexpr uint8_t BETWEEN_WALLS_GAP = screen::CANVAS_WIDTH / 2;     // Wall between differents walls
        constexpr uint8_t FIRST_WALL        = screen::CANVAS_WIDTH * 0.75;  // First wall of the game

        constexpr uint8_t LOWER_BOUND       = MINIMAL_HEIGHT;
        constexpr uint8_t UPPER_BOUND       = screen::CANVAS_HEIGHT - walls::GAP - walls::MINIMAL_HEIGHT;

        constexpr uint8_t NEW_WALL_DIFFERENTIAL = 120; // The maximum difference of between concurrent walls

        extern uint8_t displacement;

        extern int x[NUM];
        extern int y[NUM];

        extern int lastHeight;
        extern uint8_t closestToBird;

        extern TFT_eSprite spr[walls::NUM];

        void init();
        void drawSprite(uint8_t wall ,int gap_y);
        void computeNew(uint8_t index, int x);
        void reset();

    };

    namespace bird {

        constexpr int WIDTH     = 32;
        constexpr int HEIGHT    = 26;

        constexpr int X = 45;
        extern int y;

        extern TFT_eSprite spr;

        constexpr float GRAVITY = 0.5;
        constexpr float IMPULSE = -4.5;
        extern float velocity;

        void init();
        void displace(bool pressed);
        void reset();
        void draw(bool flappy, int x, int y);

    };

    namespace state {
        enum opt {
            Menu,
            Playing,
        };
        extern int current;
        extern int score;
        extern int highScore;
        extern int menuReps;
        extern bool scoreHasChanged;
        void reset();
        void advanceGame();
    }

};
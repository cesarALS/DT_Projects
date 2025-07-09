#include <TFT_eSPI.h>
#include "Fb2.h"
#include "math.h"

#define FLAP_BUTTON     13
#define RANDOM_SEED_PIN 36

// The width of the game canvas, which can be smaller than the actual display
#define CANVAS_WIDTH    235
#define CANVAS_HEIGHT   235     // For some reason, the 240 HEIGHT induces issues

#define BIRD_WIDTH    32
#define BIRD_HEIGHT   26

int walls_x[2];
int walls_y[2];

#define NUM_WALLS       2

enum GameState {
    MainMenu,
    Playing,
};

namespace walls {

    // Constants

    constexpr uint8_t WIDTH             = 30;
    constexpr uint8_t GAP               = 95;
    constexpr uint8_t MINIMAL_HEIGHT    = 20;       // The minimum height of a semi-wall

    constexpr uint8_t LOWER_BOUND       = MINIMAL_HEIGHT;
    constexpr uint8_t UPPER_BOUND       = CANVAS_HEIGHT - walls::GAP - walls::MINIMAL_HEIGHT;

    constexpr uint8_t NEW_WALL_DIFFERENTIAL = 100; // The maximum difference of between concurrent walls

    int lastHeight = CANVAS_HEIGHT/2;

};

int currentGameState = GameState::MainMenu;

int score=0;

#define GAME_SPEED      1

namespace bird {

    constexpr int X = 45;
    int y = CANVAS_HEIGHT/2;

    float gravity = 0.5;
    float impulse = -4.5;
    float velocity = 0;

    void displace(bool pressed) {
        velocity += gravity;
        y += (int)floor(velocity);
        y = max(0, min(y, CANVAS_HEIGHT-BIRD_HEIGHT));
        if(pressed){
            velocity = impulse;
        }
    }

}

TFT_eSPI tft = TFT_eSPI();

TFT_eSprite backgroundSpr = TFT_eSprite(&tft);
TFT_eSprite flappySpr = TFT_eSprite(&tft);

TFT_eSprite wallSpr[NUM_WALLS] = {
  TFT_eSprite(&tft),
  TFT_eSprite(&tft)
};

void drawWallSprite(uint8_t wall, int gapY);
void screenWipe(int speed);
int computeNewWall();

void setup() {

    Serial.begin(115200);

    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);

    backgroundSpr.createSprite(CANVAS_WIDTH, CANVAS_HEIGHT);
    backgroundSpr.setSwapBytes(true);

    flappySpr.createSprite(BIRD_WIDTH, BIRD_HEIGHT);

    for(int wall=0; wall<NUM_WALLS; wall++) {
        wallSpr[wall].createSprite(walls::WIDTH, CANVAS_HEIGHT);
        wallSpr[wall].setSwapBytes(true);
    }

    pinMode(FLAP_BUTTON, INPUT_PULLUP);

    randomSeed(analogRead(RANDOM_SEED_PIN));

    Serial.println("Starting Game!");

}

void loop() {

    if (currentGameState == GameState::MainMenu) {

        backgroundSpr.setTextColor(TFT_BLACK,TFT_CYAN);

        backgroundSpr.fillSprite(TFT_CYAN);
        backgroundSpr.drawString("Flappy Bird!", CANVAS_WIDTH/4, 10, 4);

        backgroundSpr.drawString("Press Button to Start", CANVAS_WIDTH/4, 50, 2);

        backgroundSpr.pushSprite(0,0);

        while (digitalRead(FLAP_BUTTON) == LOW);

        walls_x[0] = CANVAS_WIDTH;
        walls_y[0] = computeNewWall();
        walls_x[1] = CANVAS_WIDTH + CANVAS_WIDTH / 2;
        walls_y[1] = computeNewWall();

        Serial.println(walls_x[0]);
        Serial.println(walls_x[1]);

        while (digitalRead(FLAP_BUTTON) == HIGH);

        int score = 0;
        backgroundSpr.setTextColor(TFT_WHITE,TFT_BLUE);
        screenWipe(1000);
        currentGameState = GameState::Playing;
    }

    else if (currentGameState == GameState::Playing) {

        bird::displace(digitalRead(FLAP_BUTTON) == LOW);

        backgroundSpr.fillSprite(TFT_BLUE);

        for (int i=0; i<2; i++) {
            drawWallSprite(i, walls_y[i]);
            wallSpr[i].pushToSprite(&backgroundSpr, walls_x[i], 0, TFT_BLUE);

            if (walls_x[i] < 0) {
                walls_y[i] = computeNewWall();
                walls_x[i] = CANVAS_WIDTH;
            }

            // Serial.println(walls_x[i]);

            if (walls_x[i] == bird::X) {
                score++;
            }

            if (
                (bird::X + BIRD_WIDTH > walls_x[i] && bird::X < walls_x[i] + walls::WIDTH) // level with wall
                &&
                (bird::y < walls_y[i] || bird::y + BIRD_HEIGHT > walls_y[i] + walls::GAP) // not level with the gap
            ) {
                currentGameState = MainMenu;
            }

            walls_x[i] -= 2;
        }

        backgroundSpr.drawString(String(score),100,0,4);

        flappySpr.fillSprite(TFT_BLACK);
        flappySpr.pushImage(0,0,32,26,fb2);
        flappySpr.pushToSprite(&backgroundSpr,40,bird::y,TFT_BLACK);

        backgroundSpr.pushSprite(0,0);

        if(currentGameState == MainMenu) {
            backgroundSpr.drawString("GAME OVER", CANVAS_WIDTH/4, CANVAS_HEIGHT/2, 5);
            backgroundSpr.pushSprite(0,0);
            score = 0;
            delay(1000);
        }
        // else delay(GAME_SPEED);

    }

}

// Get the random number that will be used to create the new wall
int computeNewWall() {

    int lowerBound, upperBound;

    if(walls::lastHeight - walls::LOWER_BOUND > walls::NEW_WALL_DIFFERENTIAL/2) {
        lowerBound = walls::lastHeight - walls::NEW_WALL_DIFFERENTIAL/2;
    } else lowerBound = walls::LOWER_BOUND;

    if(walls::UPPER_BOUND - walls::lastHeight > walls::NEW_WALL_DIFFERENTIAL/2) {
        upperBound = walls::lastHeight + walls::NEW_WALL_DIFFERENTIAL/2;
    } else upperBound = walls::UPPER_BOUND;

    int newWall = random(lowerBound, upperBound);

    walls::lastHeight = newWall;
    return newWall;

}

void drawWallSprite(uint8_t wall ,int gap_y) {
    wallSpr[wall].fillSprite(TFT_BLUE);
    wallSpr[wall].fillRect(0, 0, walls::WIDTH, gap_y, TFT_GREEN);
    wallSpr[wall].fillRect(0, gap_y + walls::GAP, walls::WIDTH, CANVAS_HEIGHT - (gap_y + walls::GAP), TFT_GREEN);
}

void screenWipe(int speed) {
    for (int i = 0; i < CANVAS_HEIGHT; i += speed) {
        tft.fillRect(0, i, CANVAS_HEIGHT, speed, TFT_BLACK);
        delay(20);
    }

    for (int i = 0; i < CANVAS_HEIGHT; i += speed) {
        tft.fillRect(0, i, CANVAS_WIDTH, speed, TFT_WHITE);
        delay(20);
    }
}
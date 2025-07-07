#include <TFT_eSPI.h>
#include "Fb2.h"

#define FLAP_BUTTON     13
#define RANDOM_SEED_PIN 36

// The width of the game canvas, which can be smaller than the actual display
#define CANVAS_WIDTH    240
#define CANVAS_HEIGHT   238     // For some reason, the 240 HEIGHT induces issues

#define BIRD_WIDTH    32
#define BIRD_HEIGHT   26

#define WALL_WIDTH      30
#define WALL_GAP        95

int walls_x[2];
int walls_y[2];

enum GameState {
    MainMenu,
    Playing,
};

int currentGameState = GameState::MainMenu;

int score=0;

#define GAME_SPEED      20

namespace bird {
    constexpr int X = 45;
    int y = 15;
    int momentum;
}

TFT_eSPI tft = TFT_eSPI();

TFT_eSprite backgroundSpr = TFT_eSprite(&tft);
TFT_eSprite flappySpr = TFT_eSprite(&tft);

void screenWipe(int speed);

void setup() {

    Serial.begin(115200);

    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);

    backgroundSpr.createSprite(CANVAS_WIDTH, CANVAS_HEIGHT);
    backgroundSpr.setSwapBytes(true);

    flappySpr.createSprite(BIRD_WIDTH, BIRD_HEIGHT);

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
        walls_y[0] = CANVAS_HEIGHT / 2 - WALL_GAP / 2;
        walls_x[1] = CANVAS_WIDTH + CANVAS_WIDTH / 2;
        walls_y[1] = CANVAS_HEIGHT / 2 - WALL_GAP / 1;

        while (digitalRead(FLAP_BUTTON) == HIGH);

        int score = 0;
        backgroundSpr.setTextColor(TFT_WHITE,TFT_BLUE);
        screenWipe(1000);
        currentGameState = GameState::Playing;
    }

    else if (currentGameState == GameState::Playing) {

        if (digitalRead(FLAP_BUTTON) == LOW) {
            bird::momentum = - 5;
        }

        bird::momentum += 1;

        bird::y += bird::momentum;

        if(bird::y < 0) {
            bird::y = 0;
        }

        if (bird::y > CANVAS_HEIGHT - BIRD_HEIGHT) {
            bird::y = CANVAS_HEIGHT - BIRD_HEIGHT;
            bird::momentum = -2;
        }

        backgroundSpr.fillSprite(TFT_BLUE);

        for (int i=0; i<2; i++) {
            backgroundSpr.fillRect(walls_x[i], 0, WALL_WIDTH, walls_y[i], TFT_GREEN);
            backgroundSpr.fillRect(walls_x[i], walls_y[i] + WALL_GAP, WALL_WIDTH, CANVAS_HEIGHT-walls_y[i]+WALL_GAP, TFT_GREEN);

            if (walls_x[i] < 0) {
                walls_y[i] = random(0, CANVAS_HEIGHT - WALL_GAP);
                walls_x[i] = CANVAS_WIDTH;
            }

            if (walls_x[i] == bird::X) {
                score++;
            }

            if (
                (bird::X + BIRD_WIDTH > walls_x[i] && bird::X < walls_x[i] + WALL_WIDTH) // level with wall
                &&
                (bird::y < walls_y[i] || bird::y + BIRD_HEIGHT > walls_y[i] + WALL_GAP) // not level with the gap
            ) {
                currentGameState = MainMenu;
            }

            walls_x[i] -= 5;
        }

        backgroundSpr.drawString(String(score),100,0,2);

        flappySpr.pushImage(0,0,32,26,fb2);
        flappySpr.pushToSprite(&backgroundSpr,40,bird::y,TFT_BLACK);

        backgroundSpr.pushSprite(0,0);

        if(currentGameState == MainMenu) {
            backgroundSpr.drawString("GAME OVER", CANVAS_WIDTH/4, CANVAS_HEIGHT/2, 5);
            backgroundSpr.pushSprite(0,0);
            delay(1000);
        }
        else delay(GAME_SPEED);

    }

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
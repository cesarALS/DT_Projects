#include <TFT_eSPI.h>
#include "Fb2.h"
#include "math.h"

#define FLAP_BUTTON     13
#define RANDOM_SEED_PIN 36

#define CANVAS_WIDTH    238
#define CANVAS_HEIGHT   238     // For some reason, the 240 HEIGHT induces issues

enum GameState {
    MainMenu,
    Playing,
};
int currentGameState = GameState::MainMenu;
int score=0;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite backgroundSpr = TFT_eSprite(&tft);

namespace walls {

    constexpr uint8_t NUM = 2;

    constexpr uint8_t WIDTH             = 30;
    constexpr uint8_t GAP               = 95;
    constexpr uint8_t MINIMAL_HEIGHT    = 20;                   // The minimum height of a semi-wall
    constexpr uint8_t BETWEEN_WALLS_GAP = CANVAS_WIDTH / 2;     // Wall between differents walls
    constexpr uint8_t FIRST_WALL        = CANVAS_WIDTH * 0.75;  // First wall of the game

    constexpr uint8_t LOWER_BOUND       = MINIMAL_HEIGHT;
    constexpr uint8_t UPPER_BOUND       = CANVAS_HEIGHT - walls::GAP - walls::MINIMAL_HEIGHT;

    constexpr uint8_t NEW_WALL_DIFFERENTIAL = 110; // The maximum difference of between concurrent walls

    constexpr uint8_t DISPLACEMENT      = 3;

    int lastHeight = CANVAS_HEIGHT/2;

    int x[NUM];
    int y[NUM];

    uint8_t closestToBird = 0;

    // TODO: improve this logic, by allowing more than one wall sprite
    TFT_eSprite spr[walls::NUM] = {
        TFT_eSprite(&tft),
        TFT_eSprite(&tft)
    };

    void drawSprite(uint8_t wall ,int gap_y) {
        spr[wall].fillSprite(TFT_BLUE);
        spr[wall].fillRect(0, 0, WIDTH, gap_y, TFT_GREEN);
        spr[wall].fillRect(0, gap_y + GAP, WIDTH, CANVAS_HEIGHT - (gap_y + GAP), TFT_GREEN);
    }

    void computeNew(uint8_t index, int x) {

        int lowerBound, upperBound;

        if(walls::lastHeight - walls::LOWER_BOUND > walls::NEW_WALL_DIFFERENTIAL/2) {
            lowerBound = walls::lastHeight - walls::NEW_WALL_DIFFERENTIAL/2;
        } else lowerBound = walls::LOWER_BOUND;

        if(walls::UPPER_BOUND - walls::lastHeight > walls::NEW_WALL_DIFFERENTIAL/2) {
            upperBound = walls::lastHeight + walls::NEW_WALL_DIFFERENTIAL/2;
        } else upperBound = walls::UPPER_BOUND;

        int newWall = random(lowerBound, upperBound);

        walls::lastHeight = newWall;

        walls::x[index] = x;
        walls::y[index] = newWall;

    }

    void reset() {

        for (uint8_t wall=0; wall<NUM; wall++){
            computeNew(wall, FIRST_WALL+BETWEEN_WALLS_GAP*wall);
        }

        walls::closestToBird = 0;
    }

};

namespace bird {

    constexpr int WIDTH     = 32;
    constexpr int HEIGHT    = 26;

    constexpr int X = 45;
    int y = CANVAS_HEIGHT/2;

    TFT_eSprite spr = TFT_eSprite(&tft);

    constexpr float GRAVITY = 0.5;
    constexpr float IMPULSE = -4.5;
    float velocity = 0;

    void displace(bool pressed) {
        velocity += GRAVITY;
        y += (int)floor(velocity);
        y = max(0, min(y, CANVAS_HEIGHT-HEIGHT));
        if(pressed){
            velocity = IMPULSE;
        }
    }

    void reset() {
        y = CANVAS_HEIGHT/2;
        velocity = 0;
    }

}

void screenWipe(int speed);

void setup() {

    Serial.begin(115200);

    tft.init();
    tft.setRotation(0);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);

    backgroundSpr.createSprite(CANVAS_WIDTH, CANVAS_HEIGHT);
    backgroundSpr.setSwapBytes(true);
    backgroundSpr.setColorDepth(8);

    bird::spr.createSprite(bird::WIDTH, bird::HEIGHT);

    for(int wall=0; wall<walls::NUM; wall++) {
        walls::spr[wall].createSprite(walls::WIDTH, CANVAS_HEIGHT);
        walls::spr[wall].setSwapBytes(true);
        walls::spr[wall].setColorDepth(8);
    }

    pinMode(FLAP_BUTTON, INPUT_PULLUP);

    randomSeed(analogRead(RANDOM_SEED_PIN));

    Serial.println("Starting Game!");
    Serial.println("Height: " + String(tft.height()));
    Serial.println("Width: " + String(tft.width()));

}

void loop() {

    if (currentGameState == GameState::MainMenu) {

        backgroundSpr.setTextColor(TFT_BLACK,TFT_CYAN);

        backgroundSpr.fillSprite(TFT_CYAN);
        backgroundSpr.drawString("Flappy Bird!", CANVAS_WIDTH/4, 10, 4);

        backgroundSpr.drawString("Press Button to Start", CANVAS_WIDTH/4, 50, 2);

        backgroundSpr.pushSprite(0,0);

        while (digitalRead(FLAP_BUTTON) == LOW);

        walls::reset();
        bird::reset();

        while (digitalRead(FLAP_BUTTON) == HIGH);

        int score = 0;
        backgroundSpr.setTextColor(TFT_WHITE,TFT_BLUE);
        tft.fillScreen(TFT_BLUE);
        screenWipe(tft.height()/10);
        currentGameState = GameState::Playing;
    }

    else if (currentGameState == GameState::Playing) {

        bird::displace(digitalRead(FLAP_BUTTON) == LOW);

        backgroundSpr.fillSprite(TFT_BLUE);

        for (int i=0; i<walls::NUM; i++) {
            walls::drawSprite(i, walls::y[i]);
            walls::spr[i].pushToSprite(&backgroundSpr, walls::x[i], 0, TFT_BLUE);

            if (walls::x[i] < 0) walls::computeNew(i, CANVAS_WIDTH);

            if(i == walls::closestToBird) {
                if (walls::x[i] <= bird::X) {
                    score++;
                    walls::closestToBird = (walls::closestToBird + 1) % walls::NUM;
                }

                if (
                    (bird::X + bird::WIDTH > walls::x[i] && bird::X < walls::x[i] + walls::WIDTH) // level with wall
                    &&
                    (bird::y < walls::y[i] || bird::y + bird::HEIGHT > walls::y[i] + walls::GAP) // not level with the gap
                ) {
                    currentGameState = MainMenu;
                }

            }
            walls::x[i] -= walls::DISPLACEMENT;
        }

        backgroundSpr.drawString(String(score),100,0,4);

        bird::spr.fillSprite(TFT_BLACK);
        bird::spr.pushImage(0,0,32,26,fb2);
        bird::spr.pushToSprite(&backgroundSpr,40,bird::y,TFT_BLACK);

        backgroundSpr.pushSprite(0,0);

        if(currentGameState == MainMenu) {
            backgroundSpr.drawString("GAME OVER", CANVAS_WIDTH/4, CANVAS_HEIGHT/2, 5);
            backgroundSpr.pushSprite(0,0);
            score = 0;
            delay(1000);
        }

    }

}

void screenWipe(int speed) {
    for (int i = 0; i < tft.height(); i += speed) {
        tft.fillRect(0, i, tft.height(), speed, TFT_WHITE);
        delay(30);
    }
}
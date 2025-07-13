#include "flappy_bird.h"

namespace game {

    void init() {

        tft.init();
        tft.setRotation(0);
        tft.setSwapBytes(true);
        tft.fillScreen(TFT_GREENYELLOW);

        canvas::init();
        bird::init();
        walls::init();

        randomSeed(analogRead(RANDOM_SEED_PIN));

        Serial.println("Starting Game");
    }

    namespace canvas {
        TFT_eSprite spr = TFT_eSprite(&tft);

        void draw(){
            spr.pushSprite(PADDING, PADDING);
        }

        void init() {
            screen::initializeSprite(spr, WIDTH, HEIGHT, false);
        }
    }

    namespace state {
        int current = opt::Menu;
        int score = 0;
    }

    namespace walls {

        // TODO: improve this logic, by allowing more than one wall sprite
        TFT_eSprite spr[walls::NUM] = {
            TFT_eSprite(&tft),
            TFT_eSprite(&tft)
        };

        int x[NUM] = {0};
        int y[NUM] = {0};

        int lastHeight = canvas::HEIGHT/2;
        uint8_t closestToBird = 0;

        void init() {
            for(int wall=0; wall<walls::NUM; wall++) {
                screen::initializeSprite(spr[wall], WIDTH, canvas::HEIGHT, false);
            }
        }

        void drawSprite(uint8_t wall ,int gap_y) {
            spr[wall].fillSprite(TFT_CYAN);
            spr[wall].fillRect(0, 0, WIDTH, gap_y, TFT_GREEN);
            spr[wall].fillRect(0, gap_y + GAP, WIDTH, canvas::HEIGHT - (gap_y + GAP), TFT_GREEN);
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

            lastHeight = canvas::HEIGHT/2;
            closestToBird = 0;

        }

    };

    namespace bird {

        TFT_eSprite spr = TFT_eSprite(&tft);

        int y = canvas::HEIGHT/2;
        float velocity = 0;

        void init() {
            screen::initializeSprite(spr, WIDTH, HEIGHT, true, 16);
        }

        void displace(bool pressed) {
            velocity += GRAVITY;
            y += (int)floor(velocity);
            y = max(0, min(y, canvas::HEIGHT-HEIGHT));
            if(pressed){
                velocity = IMPULSE;
            }
        }

        void reset() {
            y = canvas::HEIGHT/2;
            velocity = 0;
        }

    }

    void advance() {

        if (state::current == state::opt::Menu) {

            tft.setTextColor(TFT_BLACK,TFT_GREENYELLOW);
            tft.drawCentreString("Flappy Bird!", tft.width()/2, 10, 4);
            tft.drawCentreString("Press Button to Start", tft.width()/2, 50, 2);

            walls::reset();
            bird::reset();

            if (button::list.at(PLAY_BUTTON)->consumeClick()) {
                state::score = 0;
                canvas::spr.setTextColor(TFT_BLACK,TFT_CYAN);
                screen::doubleWipe(5, TFT_BLACK);
                tft.fillScreen(TFT_BLACK);
                button::reset();
                state::current = state::opt::Playing;
            }
        }

        else if (state::current == state::opt::Playing) {

            bird::displace(button::list.at(PLAY_BUTTON)->consumePress());

            canvas::spr.fillSprite(TFT_CYAN);

            for (int i=0; i<walls::NUM; i++) {
                walls::drawSprite(i, walls::y[i]);
                walls::spr[i].pushToSprite(&canvas::spr, walls::x[i], 0, TFT_CYAN);

                if (walls::x[i] < 0) walls::computeNew(i, canvas::WIDTH);

                if(i == walls::closestToBird) {
                    if (walls::x[i] <= bird::X) {
                        state::score++;
                        walls::closestToBird = (walls::closestToBird + 1) % walls::NUM;
                    }

                    if (
                        (bird::X + bird::WIDTH > walls::x[i] && bird::X < walls::x[i] + walls::WIDTH) // level with wall
                        &&
                        (bird::y < walls::y[i] || bird::y + bird::HEIGHT > walls::y[i] + walls::GAP) // not level with the gap
                    ) {
                        state::current = state::opt::Menu;
                    }

                }
                walls::x[i] -= walls::DISPLACEMENT;
            }

            canvas::spr.drawCentreString(String(state::score),canvas::WIDTH/2,0,4);

            bird::spr.fillSprite(TFT_BLACK);
            bird::spr.pushImage(0,0,bird::WIDTH,bird::HEIGHT,fb2);
            bird::spr.pushToSprite(&canvas::spr,40,bird::y,TFT_BLACK);

            canvas::draw();
            if(state::current == state::opt::Menu) {
                delay(1000);
                screen::animateTextTopCenter(3, "GAME OVER", TFT_LIGHTGREY);
                delay(2000);
                screen::doubleWipe(3, TFT_GREENYELLOW);
                button::reset();
            }

        }
    }
};
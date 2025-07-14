#include "flappy_bird.h"
#include <pgmspace.h>

namespace game {

    void init() {
        canvas::init();
        bird::init();
        walls::init();
    }

    namespace canvas {

        TFT_eSprite spr = TFT_eSprite(&tft);

        const colorPalette* currentTime = &day;

        void draw(){
            spr.pushSprite(PADDING, PADDING);
        }

        void init() {
            screen::initializeSprite(spr, WIDTH, HEIGHT, false);
        }

        void staticRender() {
            canvas::spr.fillSprite(canvas::currentTime->sky);

            canvas::spr.fillSmoothCircle(
                canvas::spr.width()*0.8,
                canvas::spr.height()*0.2,
                canvas::SATELITE_RADIUS,
                canvas::currentTime->satelite,
                canvas::currentTime->sky
            );
        }

        void changeBgStyle() {
            canvas::currentTime = (bool)random(0, 2) ? &canvas::day : &canvas::night;
            canvas::spr.setTextColor(canvas::currentTime->text, canvas::currentTime->text);
        }
    }

    namespace state {
        int current = opt::Menu;
        int score = 0;
        int highScore = 0;

        int menuReps = 0;
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
            spr[wall].fillSprite(TFT_BLACK);
            spr[wall].fillRect(0, 0, WIDTH, gap_y, canvas::currentTime->wall);
            spr[wall].fillRect(0, gap_y + GAP, WIDTH, canvas::HEIGHT - (gap_y + GAP), canvas::currentTime->wall);
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

        void draw(bool flappy, int x, int y) {
            spr.fillSprite(TFT_BLACK);
            spr.pushImage(0,0,spr.width(),spr.height(), flappy ? assets::flappyB : assets::normalB);
            spr.pushToSprite(&canvas::spr,x,y,TFT_BLACK);
        }

    }

    void advance() {

        if (state::current == state::opt::Menu) {

            canvas::staticRender();
            canvas::spr.setTextColor(TFT_BLACK, TFT_BLACK);

            constexpr float base = 0.45;
            constexpr float step = 0.15;

            canvas::spr.fillRect(
                canvas::spr.width()*0.4,
                canvas::spr.height()*base,
                canvas::spr.width()*0.6,
                canvas::spr.height()*0.35,
                TFT_WHITE
            );

            canvas::spr.drawCentreString(
                "Record: " + String(state::highScore),
                canvas::spr.width()*0.7,
                canvas::spr.height()*(base+0.05),
                4
            );

            canvas::spr.drawCentreString(
                "Ultimo: " + String(state::score),
                canvas::spr.width()*0.7,
                canvas::spr.height()*(base+0.2),
                4
            );

            screen::displayButtonIndications(canvas::spr, "Juega", "Menu");

            canvas::spr.setTextColor(canvas::currentTime->text, canvas::currentTime->text);

            canvas::spr.drawCentreString(
                "JUEGO",
                canvas::spr.width()*0.35,
                canvas::spr.height()*0.15,
                4
            );

            bird::draw(
                state::menuReps % 20 < 10,
                canvas::spr.width()*0.1,
                canvas::spr.height()/2-bird::spr.height()*0.25
            );

            canvas::draw();

            walls::reset();
            bird::reset();

            if (button::list.at(PLAY_BUTTON)->consumeClick()) {
                state::score = 0;
                screen::doubleWipe(5, TFT_BLACK);
                button::reset();
                state::menuReps = 0;
                canvas::changeBgStyle();
                state::current = state::opt::Playing;
            } else {
                state::menuReps++;
            }
        }

        else if (state::current == state::opt::Playing) {

            bird::displace(button::list.at(PLAY_BUTTON)->consumePress());

            canvas::staticRender();

            for (int i=0; i<walls::NUM; i++) {
                walls::drawSprite(i, walls::y[i]);
                walls::spr[i].pushToSprite(&canvas::spr, walls::x[i], 0, TFT_BLACK);

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

            canvas::spr.drawCentreString(String(state::score),tft.width()/2,10,4);

            bird::draw(bird::velocity<0, bird::X, bird::y);

            canvas::draw();
            if(state::current == state::opt::Menu) {
                state::highScore = max(state::score, state::highScore);
                delay(1000);
                screen::animateTextTopCenter(3, "GAME OVER", TFT_YELLOW);
                delay(2000);
                screen::doubleWipe(3, canvas::currentTime->sky);
                button::reset();
            }

        }
    }
};
#include "flappy_bird.h"
#include <pgmspace.h>

namespace game {

    void init() {
        canvas::init();
        bird::init();
        walls::init();
        game::canvas::changeBgStyle();

    }

    namespace canvas {

        const colorPalette* currentTime = &day;

        void draw(){
            screen::bgSpr.pushSprite(screen::PADDING, screen::PADDING);
        }

        void init() {
            screen::initializeSprite(screen::bgSpr, screen::CANVAS_WIDTH, screen::CANVAS_HEIGHT, false);
        }

        void staticRender() {
            screen::bgSpr.fillSprite(canvas::currentTime->sky);

            screen::bgSpr.fillSmoothCircle(
                screen::bgSpr.width()*0.8,
                screen::bgSpr.height()*0.2,
                canvas::SATELITE_RADIUS,
                canvas::currentTime->satelite,
                canvas::currentTime->sky
            );
        }

        void changeBgStyle() {
            canvas::currentTime = (bool)random(0, 2) ? &canvas::day : &canvas::night;
            screen::bgSpr.setTextColor(canvas::currentTime->text, canvas::currentTime->text);
        }

        void gameMenuDisplay() {
            staticRender();
            screen::bgSpr.setTextColor(TFT_BLACK, TFT_BLACK);

            constexpr float base = 0.45;

            screen::bgSpr.fillRect(
                screen::bgSpr.width()*0.4,
                screen::bgSpr.height()*base,
                screen::bgSpr.width()*0.6,
                screen::bgSpr.height()*0.35,
                TFT_WHITE
            );

            screen::bgSpr.drawCentreString(
                "Record: " + String(state::highScore),
                screen::bgSpr.width()*0.7,
                screen::bgSpr.height()*(base+0.05),
                4
            );

            screen::bgSpr.drawCentreString(
                "Ultimo: " + String(state::score),
                screen::bgSpr.width()*0.7,
                screen::bgSpr.height()*(base+0.2),
                4
            );

            screen::displayButtonIndications(screen::bgSpr, "Juega", "Menu");

            screen::bgSpr.setTextColor(currentTime->text, currentTime->text);

            screen::bgSpr.drawCentreString(
                "JUEGO",
                screen::bgSpr.width()*0.35,
                screen::bgSpr.height()*0.15,
                4
            );

            bird::draw(
                state::menuReps % 20 < 10,
                screen::bgSpr.width()*0.1,
                screen::bgSpr.height()/2-bird::spr.height()*0.25
            );

            draw();

            state::menuReps++;

        }

        void destroySprites() {
            screen::bgSpr.deleteSprite();
            bird::spr.deleteSprite();
            for (auto &spr : walls::spr) {
                spr.deleteSprite();
            }
        }
    }

    namespace state {
        int current = opt::Menu;
        int score = 0;
        int highScore = 0;
        bool scoreHasChanged = false;

        int menuReps = 0;
        void reset() {
            walls::reset();
            bird::reset();
            button::reset();
            score = 0;
            state::menuReps = 0;
            walls::displacement = 3;
            canvas::changeBgStyle();
        }

        void advanceGame() {
            bird::displace(button::list.at(PLAY_BUTTON)->consumePress());

            canvas::staticRender();

            for (int i=0; i<walls::NUM; i++) {
                walls::drawSprite(i, walls::y[i]);
                walls::spr[i].pushToSprite(&screen::bgSpr, walls::x[i], 0, TFT_BLACK);

                if (walls::x[i] < 0) walls::computeNew(i, screen::CANVAS_WIDTH);

                if(i == walls::closestToBird) {
                    if (walls::x[i] <= bird::X) {
                        score++;
                        scoreHasChanged = true;
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
                walls::x[i] -= walls::displacement;
            }

            screen::bgSpr.drawCentreString(String(score),tft.width()/2,10,4);

            bird::draw(bird::velocity<0, bird::X, bird::y);

            canvas::draw();

            if(score>0 && scoreHasChanged && score%20==0) {
                walls::displacement++;
            }

            if(scoreHasChanged) scoreHasChanged = false;

            if(current == opt::Menu) {
                highScore = max(score, highScore);
                delay(1000);
                screen::animateTextTopCenter(3, "GAME OVER", TFT_YELLOW);
                delay(2000);
                screen::doubleWipe(3, canvas::currentTime->sky);
                button::reset();
            }
        }
    }

    namespace walls {

        // TODO: improve this logic, by allowing more than one wall sprite
        TFT_eSprite spr[walls::NUM] = {
            TFT_eSprite(&tft),
            TFT_eSprite(&tft)
        };

        int x[NUM] = {0};
        int y[NUM] = {0};

        uint8_t displacement = 3;

        int lastHeight = screen::CANVAS_HEIGHT/2;
        uint8_t closestToBird = 0;

        void init() {
            for(int wall=0; wall<walls::NUM; wall++) {
                screen::initializeSprite(spr[wall], WIDTH, screen::CANVAS_HEIGHT, false);
            }
        }

        void drawSprite(uint8_t wall ,int gap_y) {
            spr[wall].fillSprite(TFT_BLACK);
            spr[wall].fillRect(0, 0, WIDTH, gap_y, canvas::currentTime->wall);
            spr[wall].fillRect(0, gap_y + GAP, WIDTH, screen::CANVAS_HEIGHT - (gap_y + GAP), canvas::currentTime->wall);
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

            lastHeight = screen::CANVAS_HEIGHT/2;
            closestToBird = 0;

        }

    };

    namespace bird {

        TFT_eSprite spr = TFT_eSprite(&tft);

        int y = screen::CANVAS_HEIGHT/2;
        float velocity = 0;

        void init() {
            screen::initializeSprite(spr, WIDTH, HEIGHT, true, 16);
        }

        void displace(bool pressed) {
            velocity += GRAVITY;
            y += (int)floor(velocity);
            y = max(0, min(y, screen::CANVAS_HEIGHT-HEIGHT));
            if(pressed){
                velocity = IMPULSE;
            }
        }

        void reset() {
            y = screen::CANVAS_HEIGHT/2;
            velocity = 0;
        }

        void draw(bool flappy, int x, int y) {
            spr.fillSprite(TFT_BLACK);
            spr.pushImage(0,0,spr.width(),spr.height(), flappy ? assets::flappyB : assets::normalB);
            spr.pushToSprite(&screen::bgSpr,x,y,TFT_BLACK);
        }

    }
};
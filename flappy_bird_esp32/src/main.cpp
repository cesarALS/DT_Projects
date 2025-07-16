#include "globals.h"
#include "flappy_bird.h"
#include "menus.h"

void setup() {

    Serial.begin(115200);

    size_t flash_size = spi_flash_get_chip_size();
    size_t sketch_size = ESP.getSketchSize();
    size_t free_space = flash_size - sketch_size;

    Serial.printf("Total Flash: %u bytes (%.2f MB)\n", flash_size, flash_size / (1024.0 * 1024.0));
    Serial.printf("Space used by the sketch: %u bytes (%.2f KB)\n", sketch_size, sketch_size / 1024.0);
    Serial.printf("Free space: %u bytes (%.2f KB)\n", free_space, free_space / 1024.0);

    globalsInit();

    menus::list.emplace(menus::Name::Init, menus::InitMenu::menu);
    menus::list.emplace(menus::Name::MainMenu, menus::MainMenu::menu);
    menus::list.emplace(menus::Name::FlappyBird, menus::gameMenu);
    menus::list.emplace(menus::Name::Hour, menus::hourMenu);
    menus::list.emplace(menus::Name::Portrait, menus::portraitMenu);

}

void loop() {
    button::update();
    menus::advance();
}


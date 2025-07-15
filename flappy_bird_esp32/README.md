# Flappy Bird with an ESP32, using a ST7789 LCD TFT Screen

This is a simple clone of the famous and well regarded flappy bird game, implemented using the great ESP32 chip, along with a St7789 LCD TFT Screen.

The code of this implementation is **heavily** inspired in the code from nano bird: <https://gitlab.com/richardathome/nano-bird.git>

## Running it

### PlatformIO

This repo is based on the PlatformIO VSCode extension, so it is important to have it installed if you want to run it this way.

When you install it, please make sure it also installs the libraries needed to run the project, like TFT_eSPI. They will be saved in the .pio/libdeps/ folder.

You can use some other options, like the popular Arduino IDE. In that case, there are some adjustments, like changing the main.cpp to a main.ino file.

### TFT_eSPI files to modify

This project uses the TFT_eSPI library to handle communication with the TFT LCD screen. It is a highly versatile library, adaptable to many TFT LCD variants, but you should configure some files:

#### User_Setup_Select.h

You should comment the line:

```cpp
// #include <User_Setup.h>           // Default setup is root library folder
```

and then you should leave the following line uncommented:

```cpp
#include <User_Setups/Setup24_ST7789.h>            // Setup file for DSTIKE/ESP32/ESP8266 configured for ST7789 240 x 240
```

This will choose the our TFT LCD ST7789 model

#### User_Setups/Setup24_ST7789.h

This file specifically handles the ST7789 options. Because we are using an ESP32, you should have the following:

```cpp
// Generic ESP32 setup
// #define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS    -1 // Not connected
#define TFT_DC    5
#define TFT_RST   4  // Connect reset to ensure display initialises
```

This will, of course, adjust the pins. You should also physically adjust your ST7789 to be wired to those pins of the ESP32

> [!NOTE]
> Because of the library's great portability, you can run the same code in various chips, and various LCD TFT screens. You should look into them to port this code to those platforms, by changing tft_eSPI files and maybe some of the source code, like canvas sizes.

### Buttons

Finally, wire a simple button to the 32 pin of the ESP32, and another button to the 33 pin of the ESP32. Wire them both to ground also. The button connected to 32 is considered the left button, and the button connected to 33 is considered the right one.

## Wokwi simulation

You can run a simulation of the execution using the wokwi's simulated ILI9341. In that case, in order for you to run it, you need the Wokwi for VSCode extension. Also, build the project with PlatformIO, but before choose the appropiate setup in the User_Setup_Select.h file:

```cpp
#include <User_Setups/Setup42_ILI9341_ESP32.h>           // Setup file for ESP32 and SPI ILI9341 240x320
```

> [!IMPORTANT]
> Because of the complexities of the execution, wokwi will perform really bad in this simulation. It can be useful sometimes, but it will have poor performance

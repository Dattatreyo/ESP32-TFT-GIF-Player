# ESP32 Animated GIF Display on ST7735 TFT

This project demonstrates how to display animated GIFs on an **ESP32 with an ST7735 TFT display** using the **Adafruit_GFX**, **Adafruit_ST7735**, and **AnimatedGIF** libraries.

## Features
- Plays animated GIFs stored as header files
- Uses **ESP32 SPI interface** for fast display updates
- Supports **multiple GIFs** with selectable options
- **RGB565 color format** for optimized rendering

## Hardware Requirements
- **ESP32 DevKit V1**
- **1.8-inch ST7735 TFT display** (128x160 resolution)
- Jumper wires for SPI connection

## Wiring Diagram
| ESP32 Pin | TFT Display Pin |
|-----------|----------------|
| GPIO 5    | CS             |
| GPIO 4    | RST            |
| GPIO 2    | DC             |
| GPIO 18   | SCLK           |
| GPIO 23   | MOSI           |
| GPIO 15   | LED (Backlight)|

## Software Requirements
### Install the following libraries in Arduino IDE:
- **Adafruit GFX**
- **Adafruit ST7735 and ST7789**
- **AnimatedGIF**
- **SPI**

## Code Overview
### 1. Include Required Libraries and GIF Files
```cpp
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <AnimatedGIF.h>
#include "gif_header_files/zT.h"
#include "gif_header_files/automata.h"
#include "gif_header_files/shigure.h"
#include "gif_header_files/d4dj.h"
#include "gif_header_files/nyan.h"
#include "gif_header_files/mikuPLAY.h"
```

### 2. Define Pins and Initialize Display
```cpp
#define TFT_CS     5
#define TFT_RST    4
#define TFT_DC     2
#define TFT_SCLK   18
#define TFT_MOSI   23
#define TFT_LED    15
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
```

### 3. Define GIF Variables and Callback Function
```cpp
#define imageWidth  160
#define imageHeight 128
#define GIF_IMAGE mikuPLAY  // Change this to any loaded GIF file

AnimatedGIF gif;

void GIFDraw(GIFDRAW *pDraw) {
    uint16_t *usPalette = pDraw->pPalette;
    uint16_t usTemp[imageWidth];
    uint8_t *s = pDraw->pPixels;
    int iWidth = (pDraw->iWidth > tft.width()) ? tft.width() : pDraw->iWidth;
    int y = pDraw->iY + pDraw->y;
    for (int x = 0; x < iWidth; x++) {
        usTemp[x] = (pDraw->ucDisposalMethod == 2 && pDraw->ucTransparent == *s) ? BLACK : usPalette[*s];
        s++;
    }
    tft.drawRGBBitmap(pDraw->iX, y, usTemp, iWidth, 1);
}
```

### 4. Setup Function
```cpp
void setup() {
    Serial.begin(115200);
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, HIGH);
    
    tft.initR(INITR_BLACKTAB);
    tft.setSPISpeed(40000000);
    tft.setRotation(2);
    tft.fillScreen(BLACK);
    gif.begin(LITTLE_ENDIAN_PIXELS);
    
    if (!gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw)) {
        Serial.println("Error opening GIF");
    }
}
```

### 5. Loop Function
```cpp
void loop() {
    static unsigned long lastFrameTime = 0;
    static int frameDelay = 50;
    if (millis() - lastFrameTime >= frameDelay) {
        lastFrameTime = millis();
        if (!gif.playFrame(false, &frameDelay, nullptr)) {
            gif.close();
            gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw);
        }
    }
    yield();
}
```

## GIF Preparation
Since ESP32 cannot read GIFs directly from SD, **convert GIFs to C header files**:
1. Use `gif2c` tool: Converts a GIF to a C array.
2. Include generated header files in your code.

## Troubleshooting
- **GIF Not Displaying?** Check wiring and SPI speed.
- **GIF Colors Look Wrong?** Ensure proper **RGB565 format conversion**.
- **Lagging Animation?** Reduce GIF frame size or frame rate.

## Future Improvements
- Add SD card support for dynamic GIF loading
- Optimize memory usage for larger GIFs

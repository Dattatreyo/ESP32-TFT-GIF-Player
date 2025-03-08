#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>             // SPI library
#include <AnimatedGIF.h>     // AnimatedGIF library
#include "gif_header_files/zT.h"    // Include the GIF file (converted to header)
#include "gif_header_files/automata.h"
#include "gif_header_files/shigure.h"
#include "gif_header_files/d4dj.h"
#include "gif_header_files/nyan.h"
#include "gif_header_files/mikuPLAY.h"

// Pin definitions
#define TFT_CS     5
#define TFT_RST    4
#define TFT_DC     2
#define TFT_SCLK   18
#define TFT_MOSI   23
#define TFT_LED    15

// Initialize the display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Color definitions
#define BLACK   0x0000

// GIF object
AnimatedGIF gif;

// Image dimensions and GIF selection
#define imageWidth  160
#define imageHeight 128
// Uncomment one of the following as needed:
// #define GIF_IMAGE automata //rotation 2
#define GIF_IMAGE mikuPLAY

// Variables for frame timing
unsigned long lastFrameTime = 0;
int frameDelay = 50;  // Initial frame delay; will be updated by GIF data

// GIF drawing callback: Draws one horizontal line of the GIF image
void GIFDraw(GIFDRAW *pDraw) {
  uint16_t *usPalette = pDraw->pPalette;
  uint16_t usTemp[imageWidth]; // Buffer for one full line
  uint8_t *s = pDraw->pPixels;
  int iWidth = pDraw->iWidth > tft.width() ? tft.width() : pDraw->iWidth;
  int y = pDraw->iY + pDraw->y; // Current line position

  // Convert each pixel in the line to RGB565
  for (int x = 0; x < iWidth; x++) {
    // Use background color (BLACK) if this pixel is transparent (and disposal method indicates clearing)
    usTemp[x] = (pDraw->ucDisposalMethod == 2 && pDraw->ucTransparent == *s) ? BLACK : usPalette[*s];
    s++;
  }

  // Draw the full line at once
  tft.drawRGBBitmap(pDraw->iX, y, usTemp, iWidth, 1);
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 is starting up");

  // Turn on the TFT backlight
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  Serial.println("TFT screen initializing");
  // Initialize display and set rotation/speed
  tft.initR(INITR_BLACKTAB);
  tft.setSPISpeed(40000000);  // Use 40MHz for faster updates
  tft.setRotation(2);         // Landscape mode

  // Clear the screen
  tft.fillScreen(BLACK);

  // Initialize the GIF library
  gif.begin(LITTLE_ENDIAN_PIXELS);

  Serial.println("Setup complete, starting animation");
  // Open the GIF file
  if (!gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw)) {
    Serial.println("Error opening GIF");
  }
}

void loop() {
  // Update the GIF frame only when the frameDelay has elapsed
  if (millis() - lastFrameTime >= frameDelay) {
    lastFrameTime = millis();
    // Play one frame; pass &frameDelay so the library can adjust the delay per frame
    if (gif.playFrame(false, &frameDelay, nullptr)) {
      // The frameDelay variable is updated automatically based on the GIF frame's delay.
    } else {
      // If the GIF has finished, restart it seamlessly
      gif.close();
      gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw);
    }
  }
  yield();  // Allow background tasks to run
}

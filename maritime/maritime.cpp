#include "shared/module.h"
#include "shared/debounce.h"

#include <Adafruit_GFX.h>
#include <XTronical_ST7735.h> // Hardware-specific library
#include <SPI.h>

#include "flags.h"

#include "AlphaNum4.h"

const Address addr = address::MARITIME;
const uint8_t PIN_DISARM_LED = 12;

// set up pins we are going to use to talk to the screen
const uint8_t TFT_SCLK = 13; // SPI clock
const uint8_t TFT_MOSI = 11; // SPI Data
const uint8_t TFT_CS   = 10; // Display enable (Chip select), if not enabled will not talk on SPI bus
const uint8_t TFT_RST  =  9; // Display reset pin, you can also connect this to the Arduino reset
                            // in which case, set this #define pin to -1!
const uint8_t TFT_DC   =  8; // register select (stands for Data Control perhaps!)

const uint8_t PIN_SEED = A0;
const uint8_t PIN_SOFT_SDA = A2;
const uint8_t PIN_SOFT_SCL = A3;
const uint8_t PIN_DOWN = 4;
const uint8_t PIN_UP = 5;
const uint8_t PIN_NEXT = 6;
Debounce down(PIN_DOWN);
Debounce up(PIN_UP);
Debounce next(PIN_NEXT);

const uint8_t ALPHA_ADDRESS = 0x70;
AlphaNum4 alpha4 = AlphaNum4(PIN_SOFT_SDA, PIN_SOFT_SCL);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

char code[4] = {'A'};

const char valid[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
size_t progress;
size_t current;

void initialise()
{
  randomSeed(analogRead(PIN_SEED));
  up.init();
  down.init();
  next.init();

  // start the i2c display
  alpha4.begin(ALPHA_ADDRESS);
  alpha4.clear();
  alpha4.writeDisplay();

  tft.init();
  tft.fillScreen(COLOUR_BLACK);
  tft.setRotation(2);

  reset();
}

void reset()
{
  alpha4.clear();
  alpha4.writeDisplay();
  tft.fillScreen(COLOUR_BLACK);

  progress = 0;
  current = 0;

  // randomize the code
  for (size_t i = 0; i < 4; ++i)
    code[i] = valid[random(0, sizeof(valid))];

  state = ModuleState::READY;
}

void onIndicators()
{
}

void arm()
{
  flag(tft, code[0]);
}

void idle()
{
  up.update();
  down.update();
  next.update();

  for (size_t c = 0; c < progress; ++c)
    alpha4.writeDigitAscii(c, code[c], false);

  alpha4.writeDigitAscii(progress, valid[current], true);
  alpha4.writeDisplay();

  if (up.is_released())
  {
    current = (current + 1) % sizeof(valid);
  }
  if (down.is_released())
  {
    current = (current + sizeof(valid) - 1) % sizeof(valid);
  }
  if (next.is_released())
  {
    if (valid[current] == code[progress])
    {
      ++progress;
      if (progress == 4)
      {
        disarm();
        tft.fillScreen(COLOUR_BLACK);
      }
      else
      {
        current = 0;
        flag(tft, code[progress]);
      }
    }
    else
    {
      strike();
    }
  }
}

void defuse()
{
}

void detonate()
{
}

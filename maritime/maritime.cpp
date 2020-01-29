#include "shared/module.h"
#include "shared/debounce.h"

#include <Adafruit_GFX.h>
#include <XTronical_ST7735.h> // Hardware-specific library
#include <SPI.h>

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

const uint8_t PIN_DOWN = 4;
const uint8_t PIN_UP = 5;
const uint8_t PIN_NEXT = 6;

AlphaNum4 alpha4 = AlphaNum4(A2, A3);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

char code[4] = {'A'};

const uint8_t R_MASK = 0b11111;
const uint8_t G_MASK = 0b111111;
const uint8_t B_MASK = 0b11111;
constexpr uint16_t colour(const float r, const float g, const float b)
{
  return static_cast<uint16_t>(static_cast<uint8_t>(r * R_MASK) & R_MASK) << 11 |
    static_cast<uint16_t>(static_cast<uint8_t>(g * G_MASK) & G_MASK) << 5 |
    static_cast<uint16_t>(static_cast<uint8_t>(b * B_MASK) & B_MASK);
}

const uint16_t COLOUR_BLACK   = 0x0000;
const uint16_t COLOUR_BLUE    = 0x001F;
const uint16_t COLOUR_RED     = 0xF800;
const uint16_t COLOUR_GREEN   = 0x07E0;
const uint16_t COLOUR_CYAN    = 0x07FF;
const uint16_t COLOUR_MAGENTA = 0xF81F;
const uint16_t COLOUR_YELLOW  = 0xFFE0;
const uint16_t COLOUR_WHITE   = 0xFFFF;
const uint16_t COLOUR_GREY    = colour(0.75f, 0.75f, 0.75f);

void flag(const char f)
{
  const int16_t width = tft.width();
  const int16_t height = tft.height();

  if (isdigit(f))
  {
    tft.fillScreen(COLOUR_GREY);

    switch (f)
    {
      case '0':
        tft.fillRect(0, height / 5.0f, width, height / 5.0f * 3.0f, COLOUR_YELLOW);
        tft.fillRect(width / 3.0f, height / 5.0f, width / 3.0f, height / 5.0f * 3.0f, COLOUR_RED);
      break;
      case '1':
        tft.fillRect(0, height / 5.0f, width, height / 5.0f * 3.0f, COLOUR_WHITE);
        tft.fillCircle(width / 4.0f, height / 2.0f, width / 8.0f, COLOUR_RED);
      break;
      case '2':
        tft.fillRect(0, height / 5.0f, width, height / 5.0f * 3.0f, COLOUR_BLUE);
        tft.fillCircle(width / 4.0f, height / 2.0f, width / 8.0f, COLOUR_WHITE);
      break;
      case '3':
        tft.fillRect(0, height / 5.0f, width / 3.0f, height / 5.0f * 3.0f, COLOUR_RED);
        tft.fillRect(width / 3.0f, height / 5.0f, width / 3.0f, height / 5.0f * 3.0f, COLOUR_WHITE);
        tft.fillRect(width / 3.0f * 2.0f, height / 5.0f, width / 3.0f, height / 5.0f * 3.0f, COLOUR_BLUE);
      break;
      case '4':
        tft.fillRect(0, height / 5.0f, width, height / 5.0f * 3.0f, COLOUR_RED);
        tft.fillRect(0, height / 2.0f - height / 20.0f, width, height / 10.0f, COLOUR_WHITE);
        tft.fillRect(width / 10.0f * 2.0f, height / 5.0f, width / 10.0f, height / 5.0f * 3.0f, COLOUR_WHITE);
      break;
      case '5':
        tft.fillRect(0, height / 5.0f, width / 2.0f, height / 5.0f * 3.0f, COLOUR_YELLOW);
        tft.fillRect(width / 2.0f, height / 5.0f, width / 2.0f, height / 5.0f * 3.0f, COLOUR_BLUE);
      break;
      case '6':
        tft.fillRect(0, height / 5.0f, width, height / 10.0f * 3.0f, COLOUR_BLACK);
        tft.fillRect(0, height / 2.0f, width, height / 10.0f * 3.0f, COLOUR_WHITE);
      break;
      case '7':
        tft.fillRect(0, height / 5.0f, width, height / 10.0f * 3.0f, COLOUR_YELLOW);
        tft.fillRect(0, height / 2.0f, width, height / 10.0f * 3.0f, COLOUR_RED);
      break;
      case '8':
        tft.fillRect(0, height / 5.0f, width, height / 5.0f * 3.0f, COLOUR_WHITE);
        tft.fillRect(0, height / 2.0f - height / 20.0f, width, height / 10.0f, COLOUR_RED);
        tft.fillRect(width / 10.0f * 2.0f, height / 5.0f, width / 10.0f, height / 5.0f * 3.0f, COLOUR_RED);
      break;
      case '9':
        tft.fillRect(0, height / 5.0f, width / 2.0f, height / 10.0f * 3.0f, COLOUR_WHITE);
        tft.fillRect(width / 2.0f, height / 5.0f, width / 2.0f, height / 10.0f * 3.0f, COLOUR_BLACK);
        tft.fillRect(0, height / 2.0f, width / 2.0f, height / 10.0f * 3.0f, COLOUR_RED);
        tft.fillRect(width / 2.0f, height / 2.0f, width / 2.0f, height / 10.0f * 3.0f, COLOUR_YELLOW);
      break;
    }

    tft.fillTriangle(0, height / 5.0f, width, height / 5.0f, width, (height / 10.0f) * 3.5f, COLOUR_GREY);
    tft.fillTriangle(0, height / 5.0f * 4.0f, width, height / 5.0f * 4.0f, width, (height / 10.0f) * 6.5f, COLOUR_GREY);

    return;
  }

  switch (f)
  {
    case 'A':
      tft.fillScreen(COLOUR_GREY);
      tft.fillRect(0, 0, width / 2, height, COLOUR_WHITE);
      tft.fillRect(width / 2, 0, width / 4, height, COLOUR_BLUE);
      tft.fillTriangle(width / 2 + width / 4, 0, width, 0, width / 2 + width / 4, height / 2, COLOUR_BLUE);
      tft.fillTriangle(width / 2 + width / 4, height, width, height, width / 2 + width / 4, height / 2, COLOUR_BLUE);
    break;
    case 'B':
      tft.fillScreen(COLOUR_GREY);
      tft.fillRect(0, 0, width / 2 + width / 4, height, COLOUR_RED);
      tft.fillTriangle(width / 2 + width / 4, 0, width, 0, width / 2 + width / 4, height / 2, COLOUR_RED);
      tft.fillTriangle(width / 2 + width / 4, height, width, height, width / 2 + width / 4, height / 2, COLOUR_RED);
    break;
    case 'C':
      tft.fillRect(0, 0, width, height / 5, COLOUR_BLUE);
      tft.fillRect(0, height / 5, width, height / 5, COLOUR_WHITE);
      tft.fillRect(0, height / 5 * 2, width, height / 5, COLOUR_RED);
      tft.fillRect(0, height / 5 * 3, width, height / 5, COLOUR_WHITE);
      tft.fillRect(0, height / 5 * 4, width, height / 5, COLOUR_BLUE);
    break;
    case 'D':
      tft.fillRect(0, 0, width, height / 5, COLOUR_YELLOW);
      tft.fillRect(0, height / 5, width, height / 5 * 3, COLOUR_BLUE);
      tft.fillRect(0, height / 5 * 4, width, height / 5, COLOUR_YELLOW);
    break;
    case 'E':
      tft.fillRect(0, 0, width, height / 2, COLOUR_BLUE);
      tft.fillRect(0, height / 2, width, height, COLOUR_RED);
    break;
    case 'F':
      tft.fillScreen(COLOUR_RED);
      tft.fillTriangle(0, 0, width / 2, 0, 0, height / 2, COLOUR_WHITE);
      tft.fillTriangle(width, 0, width / 2, 0, width, height / 2, COLOUR_WHITE);
      tft.fillTriangle(0, height, width / 2, height, 0, height / 2, COLOUR_WHITE);
      tft.fillTriangle(width, height, width / 2, height, width, height / 2, COLOUR_WHITE);
    break;
    case 'G':
      tft.fillRect(0, 0, width / 6, height, COLOUR_YELLOW);
      tft.fillRect(width / 6, 0, width / 6, height, COLOUR_BLUE);
      tft.fillRect(width / 6 * 2, 0, width / 6, height, COLOUR_YELLOW);
      tft.fillRect(width / 6 * 3, 0, width / 6, height, COLOUR_BLUE);
      tft.fillRect(width / 6 * 4, 0, width / 6, height, COLOUR_YELLOW);
      tft.fillRect(width / 6 * 5, 0, width / 6, height, COLOUR_BLUE);
    break;
    case 'H':
      tft.fillRect(0, 0, width / 2, height, COLOUR_WHITE);
      tft.fillRect(width / 2, 0, width / 2, height, COLOUR_RED);
    break;
    case 'I':
      tft.fillScreen(COLOUR_YELLOW);
      tft.fillCircle(width / 2, height / 2, width / 5, COLOUR_BLACK);
    break;
    case 'J':
      tft.fillRect(0, 0, width, height / 3, COLOUR_BLUE);
      tft.fillRect(0, height / 3, width, height / 3, COLOUR_WHITE);
      tft.fillRect(0, height / 3 * 2, width, height / 3, COLOUR_BLUE);
    break;
    case 'K':
      tft.fillRect(0, 0, width / 2, height, COLOUR_YELLOW);
      tft.fillRect(width / 2, 0, width / 2, height, COLOUR_BLUE);
    break;
    case 'L':
      tft.fillScreen(COLOUR_BLACK);
      tft.fillRect(0, 0, width / 2, height / 2, COLOUR_YELLOW);
      tft.fillRect(width / 2, height / 2, width / 2, height / 2, COLOUR_YELLOW);
    break;
    case 'M':
      tft.fillScreen(COLOUR_WHITE);
      tft.fillTriangle(width / 8, 0, width / 8 * 7, 0, width / 2, height / 8 * 3, COLOUR_BLUE);
      tft.fillTriangle(0, height / 8, 0, height / 8 * 7, width / 8 * 3, height / 2, COLOUR_BLUE);
      tft.fillTriangle(width / 8, height, width / 8 * 7, height, width / 2, height / 8 * 5, COLOUR_BLUE);
      tft.fillTriangle(width, height / 8, width, height / 8 * 7, width / 8 * 5, height / 2, COLOUR_BLUE);
    break;
    case 'N':
      tft.fillScreen(COLOUR_WHITE);
      // top row
      tft.fillRect(0, 0, width / 4, height / 4, COLOUR_BLUE);
      tft.fillRect(width / 2, 0, width / 4, height / 4, COLOUR_BLUE);
      // second row
      tft.fillRect(width / 4, height / 4, width / 4, height / 4, COLOUR_BLUE);
      tft.fillRect(width / 4 * 3, height / 4, width / 4, height / 4, COLOUR_BLUE);
      // third row
      tft.fillRect(0, height / 2, width / 4, height / 4, COLOUR_BLUE);
      tft.fillRect(width / 2, height / 2, width / 4, height / 4, COLOUR_BLUE);
      // second row
      tft.fillRect(width / 4, height / 4 * 3, width / 4, height / 4, COLOUR_BLUE);
      tft.fillRect(width / 4 * 3, height / 4 * 3, width / 4, height / 4, COLOUR_BLUE);
    break;
    case 'O':
      tft.fillTriangle(0, 0, width, 0, width, height, COLOUR_RED);
      tft.fillTriangle(0, 0, 0, height, width, height, COLOUR_YELLOW);
    break;
    case 'P':
      tft.fillScreen(COLOUR_BLUE);
      tft.fillRect(width / 3, height / 3, width / 3, height / 3, COLOUR_WHITE);
    break;
    case 'Q':
      tft.fillScreen(COLOUR_YELLOW);
    break;
    case 'R':
      tft.fillScreen(COLOUR_RED);
      tft.fillRect(width / 7 * 3, 0, width / 7, height, COLOUR_YELLOW);
      tft.fillRect(0, height / 7 * 3, width, height / 7, COLOUR_YELLOW);
    break;
    case 'S':
      tft.fillScreen(COLOUR_WHITE);
      tft.fillRect(width / 3, height / 3, width / 3, height / 3, COLOUR_BLUE);
    break;
    case 'T':
      tft.fillRect(0, 0, width / 3, height, COLOUR_RED);
      tft.fillRect(width / 3, 0, width / 3, height, COLOUR_WHITE);
      tft.fillRect(width / 3 * 2, 0, width / 3, height, COLOUR_BLUE);
    break;
    case 'U':
      tft.fillScreen(COLOUR_WHITE);
      tft.fillRect(0, 0, width / 2, height / 2, COLOUR_RED);
      tft.fillRect(width / 2, height / 2, width / 2, height / 2, COLOUR_RED);
    break;
    case 'V':
      tft.fillScreen(COLOUR_RED);
      tft.fillTriangle(width / 8, 0, width / 8 * 7, 0, width / 2, height / 8 * 3, COLOUR_WHITE);
      tft.fillTriangle(0, height / 8, 0, height / 8 * 7, width / 8 * 3, height / 2, COLOUR_WHITE);
      tft.fillTriangle(width / 8, height, width / 8 * 7, height, width / 2, height / 8 * 5, COLOUR_WHITE);
      tft.fillTriangle(width, height / 8, width, height / 8 * 7, width / 8 * 5, height / 2, COLOUR_WHITE);
    break;
    case 'W':
      tft.fillScreen(COLOUR_BLUE);
      tft.fillRect(width / 5, height / 5, width / 5 * 3, height / 5 * 3, COLOUR_WHITE);
      tft.fillRect(width / 5 * 2, height / 5 * 2, width / 5, height / 5, COLOUR_RED);
    break;
    case 'X':
      tft.fillScreen(COLOUR_WHITE);
      tft.fillRect(width / 7 * 3, 0, width / 7, height, COLOUR_BLUE);
      tft.fillRect(0, height / 7 * 3, width, height / 7, COLOUR_BLUE);
    break;
    case 'Y':
      // top left
      tft.fillTriangle(width, 0, 0, height, 0, 0, COLOUR_YELLOW);
      tft.fillTriangle(width / 5 * 4, 0, 0, height / 5 * 4, 0, 0, COLOUR_RED);
      tft.fillTriangle(width / 5 * 3, 0, 0, height / 5 * 3, 0, 0, COLOUR_YELLOW);
      tft.fillTriangle(width / 5 * 2, 0, 0, height / 5 * 2, 0, 0, COLOUR_RED);
      tft.fillTriangle(width / 5, 0, 0, height / 5, 0, 0, COLOUR_YELLOW);
      // bottom right
      tft.fillTriangle(width, 0, 0, height, width, height, COLOUR_RED);
      tft.fillTriangle(width / 5, height, width, height / 5, width, height, COLOUR_YELLOW);
      tft.fillTriangle(width / 5 * 2, height, width, height / 5 * 2, width, height, COLOUR_RED);
      tft.fillTriangle(width / 5 * 3, height, width, height / 5 * 3, width, height, COLOUR_YELLOW);
      tft.fillTriangle(width / 5 * 4, height, width, height / 5 * 4, width, height, COLOUR_RED);
    break;
    case 'Z':
      tft.fillTriangle(0, 0, width, 0, width / 2, height / 2, COLOUR_YELLOW);
      tft.fillTriangle(width, 0, width, height, width / 2, height / 2, COLOUR_BLUE);
      tft.fillTriangle(width, height, 0, height, width / 2, height / 2, COLOUR_RED);
      tft.fillTriangle(0, height, 0, 0, width / 2, height / 2, COLOUR_BLACK);
    break;
    default:
      tft.fillScreen(COLOUR_MAGENTA);
    break;
  }
}

const char valid[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
size_t progress;
size_t current;

void initialise()
{
  Serial.begin(9600);
  randomSeed(analogRead(PIN_SEED));

  // start the i2c display
  alpha4.begin(address::MARITIME_ALPHA);

  tft.init();
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
    code[i] = random(0, 2) ? random('0', '9' + 1) : random('A', 'Z' + 1);

  state = ModuleState::READY;
}

void onIndicators()
{
}

void arm()
{
  flag(code[0]);
}

void idle()
{
  for (size_t c = 0; c < progress; ++c)
    alpha4.writeDigitAscii(c, code[c], false);

  alpha4.writeDigitAscii(progress, valid[current], true);
  alpha4.writeDisplay();

  if (digitalRead(PIN_UP))
  {
    while (digitalRead(PIN_UP));
    current = (current + 1) % sizeof(valid);
  }
  if (digitalRead(PIN_DOWN))
  {
    while (digitalRead(PIN_DOWN));
    current = (current + sizeof(valid) - 1) % sizeof(valid);
  }
  if (digitalRead(PIN_NEXT))
  {
    while (digitalRead(PIN_NEXT));
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
        flag(code[progress]);
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

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <XTronical_ST7735.h> // Hardware-specific library
#include <SPI.h>

// set up pins we are going to use to talk to the screen
const uint8_t TFT_SCLK = 13; // SPI clock
const uint8_t TFT_MOSI = 11; // SPI Data
const uint8_t TFT_CS   = 10; // Display enable (Chip select), if not enabled will not talk on SPI bus
const uint8_t TFT_RST  =  9; // Display reset pin, you can also connect this to the Arduino reset
                            // in which case, set this #define pin to -1!
const uint8_t TFT_DC   =  8; // register select (stands for Data Control perhaps!)

const uint8_t PIN_SEE = A0;

const uint8_t PIN_DOWN = 4;
const uint8_t PIN_UP = 5;
const uint8_t PIN_NEXT = 6;

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
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
    case '0':
      tft.fillScreen(COLOUR_GREY);
      tft.fillRect(0, height / 5, width, height / 5 * 3, COLOUR_YELLOW);
      tft.fillRect(width / 3, height / 5, width / 3, height / 5 * 3, COLOUR_RED);
      tft.fillTriangle(0, height / 5, width, height / 5, width, (height / 10.0f) * 3.5f, COLOUR_GREY);
      tft.fillTriangle(0, height / 5 * 4, width, height / 5 * 4, width, (height / 10.0f) * 6.5f, COLOUR_GREY);
    break;
    case '1':
      tft.fillScreen(COLOUR_GREY);
      tft.fillRect(0, height / 5.0f, width, height / 5.0f * 3.0f, COLOUR_WHITE);
      tft.fillCircle(width / 4.0f, height / 2.0f, width / 8.0f, COLOUR_RED);
      tft.fillTriangle(0, height / 5, width, height / 5, width, height / 10.0f * 3.5f, COLOUR_GREY);
      tft.fillTriangle(0, height / 5 * 4, width, height / 5 * 4, width, height / 10.0f * 6.5f, COLOUR_GREY);
    break;
    case '2':
      tft.fillScreen(COLOUR_GREY);
      tft.fillRect(0, height / 5.0f, width, height / 5.0f * 3.0f, COLOUR_BLUE);
      tft.fillCircle(width / 4.0f, height / 2.0f, width / 8.0f, COLOUR_WHITE);
      tft.fillTriangle(0, height / 5, width, height / 5, width, height / 10.0f * 3.5f, COLOUR_GREY);
      tft.fillTriangle(0, height / 5 * 4, width, height / 5 * 4, width, height / 10.0f * 6.5f, COLOUR_GREY);
    break;
    case '3':
      tft.fillScreen(COLOUR_GREY);
      tft.fillRect(0, height / 5.0f, width / 3.0f, height / 5.0f * 3.0f, COLOUR_RED);
      tft.fillRect(width / 3.0f, height / 5.0f, width / 3.0f, height / 5.0f * 3.0f, COLOUR_WHITE);
      tft.fillRect(width / 3.0f * 2.0f, height / 5.0f, width / 3.0f, height / 5.0f * 3.0f, COLOUR_BLUE);
      tft.fillTriangle(0, height / 5, width, height / 5, width, (height / 10.0f) * 3.5f, COLOUR_GREY);
      tft.fillTriangle(0, height / 5 * 4, width, height / 5 * 4, width, (height / 10.0f) * 6.5f, COLOUR_GREY);
    break;
    case '4':
      tft.fillScreen(COLOUR_GREY);
      tft.fillRect(0, height / 5.0f, width, height / 5.0f * 3.0f, COLOUR_RED);
      tft.fillRect(0, height / 2.0f - height / 20.0f, width, height / 10.0f, COLOUR_WHITE);
      tft.fillRect(width / 10.0f * 2.0f, height / 5.0f, width / 10.0f, height / 5.0f * 3.0f, COLOUR_WHITE);
      tft.fillTriangle(0, height / 5, width, height / 5, width, (height / 10.0f) * 3.5f, COLOUR_GREY);
      tft.fillTriangle(0, height / 5 * 4, width, height / 5 * 4, width, (height / 10.0f) * 6.5f, COLOUR_GREY);
    break;
    case '5':
      tft.fillScreen(COLOUR_GREY);
      tft.fillRect(0, height / 5.0f, width / 2.0f, height / 5.0f * 3.0f, COLOUR_YELLOW);
      tft.fillRect(width / 2.0f, height / 5.0f, width / 2.0f, height / 5.0f * 3.0f, COLOUR_BLUE);
      tft.fillTriangle(0, height / 5.0f, width, height / 5.0f, width, (height / 10.0f) * 3.5f, COLOUR_GREY);
      tft.fillTriangle(0, height / 5.0f * 4, width, height / 5.0f * 4, width, (height / 10.0f) * 6.5f, COLOUR_GREY);
    break;
    case '6':
      tft.fillScreen(COLOUR_GREY);
      tft.fillRect(0, height / 5.0f, width, height / 2.0f - height / 5.0f, COLOUR_BLACK);
      tft.fillRect(0, height / 2.0f, width, height / 2.0f - height / 5.0f, COLOUR_WHITE);
      tft.fillTriangle(0, height / 5, width, height / 5, width, (height / 10.0f) * 3.5f, COLOUR_GREY);
      tft.fillTriangle(0, height / 5 * 4, width, height / 5 * 4, width, (height / 10.0f) * 6.5f, COLOUR_GREY);
    break;
    case '7':
      tft.fillScreen(COLOUR_GREY);
      tft.fillRect(0, height / 5.0f, width, height / 2.0f - height / 5.0f, COLOUR_YELLOW);
      tft.fillRect(0, height / 2.0f, width, height / 2.0f - height / 5.0f, COLOUR_RED);
      tft.fillTriangle(0, height / 5, width, height / 5, width, (height / 10.0f) * 3.5f, COLOUR_GREY);
      tft.fillTriangle(0, height / 5 * 4, width, height / 5 * 4, width, (height / 10.0f) * 6.5f, COLOUR_GREY);
    break;
    case '8':
      tft.fillScreen(COLOUR_GREY);
      tft.fillRect(0, height / 5.0f, width, height / 5.0f * 3.0f, COLOUR_WHITE);
      tft.fillRect(0, height / 2.0f - height / 20.0f, width, height / 10.0f, COLOUR_RED);
      tft.fillRect(width / 10.0f * 2.0f, height / 5.0f, width / 10.0f, height / 5.0f * 3.0f, COLOUR_RED);
      tft.fillTriangle(0, height / 5, width, height / 5, width, (height / 10.0f) * 3.5f, COLOUR_GREY);
      tft.fillTriangle(0, height / 5 * 4, width, height / 5 * 4, width, (height / 10.0f) * 6.5f, COLOUR_GREY);
    break;
    case '9':
      tft.fillScreen(COLOUR_GREY);
      tft.fillRect(0, height / 5.0f, width / 2.0f, height / 2.0f - height / 5.0f, COLOUR_WHITE);
      tft.fillRect(width / 2.0f, height / 5.0f, width / 2.0f, height / 2.0f - height / 5.0f, COLOUR_BLACK);
      tft.fillRect(0, height / 2.0f, width / 2.0f, height / 2.0f - height / 5.0f, COLOUR_RED);
      tft.fillRect(width / 2.0f, height / 2.0f, width / 2.0f, height / 2.0f - height / 5.0f, COLOUR_YELLOW);
      tft.fillTriangle(0, height / 5, width, height / 5, width, (height / 10.0f) * 3.5f, COLOUR_GREY);
      tft.fillTriangle(0, height / 5 * 4, width, height / 5 * 4, width, (height / 10.0f) * 6.5f, COLOUR_GREY);
    break;

    default:
    break;
  }
}

void setup()
{
  Serial.begin(9600);

  randomSeed(analogRead(PIN_SEE));

  // start the i2c display
  alpha4.begin(0x70);

  alpha4.clear();
  alpha4.writeDisplay();

  // start the tft
  tft.init();
  tft.setRotation(2);

  // randomize the code
  for (size_t i = 0; i < 4; ++i)
    code[i] = random(0, 1) ? random('0', '9' + 1) : random('A', 'Z' + 1);

  for (size_t i = 0; i < 4; ++i)
    Serial.print(code[i]);
  Serial.println("");
}


char displaybuffer[4] = {' ', ' ', ' ', ' '};

void loop()
{
  for (char a = '0'; a <= '9'; ++a)
  {
    flag(a);
    delay(2500);
  }

/*
  // scroll down display
  displaybuffer[0] = displaybuffer[1];
  displaybuffer[1] = displaybuffer[2];
  displaybuffer[2] = displaybuffer[3];
  displaybuffer[3] = c;

  // set every digit to the buffer
  alpha4.writeDigitAscii(0, displaybuffer[0]);
  alpha4.writeDigitAscii(1, displaybuffer[1]);
  alpha4.writeDigitAscii(2, displaybuffer[2]);
  alpha4.writeDigitAscii(3, displaybuffer[3]);

  // write it out!
  alpha4.writeDisplay();
  delay(200);
*/
}

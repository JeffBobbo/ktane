/* Demo for XTronical 7735 driver library for generic 128x128 pixel
   TFT displays, based on the one from ADAFruit, their original
   header comments are below.
 */

/***************************************************
  This is a library for the genric 1.8" SPI display.

This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <Adafruit_GFX.h>    // Core graphics library
#include <XTronical_ST7735.h> // Hardware-specific library
#include <SPI.h>

// set up pins we are going to use to talk to the screen
const uint8_t TFT_SCLK = 13; // SPI clock
const uint8_t TFT_MOSI = 11; // SPI Data
const uint8_t TFT_CS   = 10; // Display enable (Chip select), if not enabled will not talk on SPI bus
const uint8_t TFT_RST  =  9; // Display reset pin, you can also connect this to the Arduino reset
                            // in which case, set this #define pin to -1!
const uint8_t TFT_DC   =  8; // register select (stands for Data Control perhaps!)

const uint8_t ANALOG_X = A0;
const uint8_t ANALOG_Y = A1;
const uint8_t ANALOG_FLOAT = A2;

// initialise the routine to talk to this display with these pin connections (as we've missed off
// TFT_SCLK and TFT_MOSI the routine presumes we are using hardware SPI and internally uses 13 and 11
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);  

float p = 3.1415926;

const uint8_t R_MASK = 0b11111;
const uint8_t G_MASK = 0b111111;
const uint8_t B_MASK = 0b11111;
constexpr uint16_t colour(const float r, const float g, const float b)
{
  return static_cast<uint16_t>(static_cast<uint8_t>(r * R_MASK) & R_MASK) << 11 | 
    static_cast<uint16_t>(static_cast<uint8_t>(g * G_MASK) & G_MASK) << 5 |
    static_cast<uint16_t>(static_cast<uint8_t>(b * B_MASK) & B_MASK);
}

constexpr float pi()
{
  return 3.14159265359f;
}

const uint16_t COLOUR_BEAM = colour(0.25f, 0.75f, 0.25f);
uint16_t COLOURS[] = {
  colour(1.0f, 0.0f, 0.0f),
  colour(0.0f, 0.0f, 1.0f),
  colour(1.0f, 1.0f, 0.0f),
  colour(0.0f, 1.0f, 0.0f),
};

const float BEAM_WIDTH = pi() / 16.0f;

class Target
{
public:
  Target() {}
  Target(const int8_t _x, const int8_t _y, const uint16_t c, const float a) : x(_x), y(_y), colour(c), angle(a), rendered(false) {}

  bool inRange(const float l, const float r) const
  {
    if (l <= r)
    {
      if (l < angle && angle < r)
        return true;
    }
    else
    {
      if (angle > l && angle > r)
        return true;
      if (angle < l && angle < r)
        return true;
    }
    return false;
  }

  void render()
  {
    if (!rendered)
    {
      tft.fillCircle(x, y, 4, colour);
      rendered = true;
    }
  }

  void clear()
  {
    if (rendered)
    {
      tft.fillCircle(x, y, 4, 0x0000);
      rendered = false;
    }
  }

  int8_t x;
  int8_t y;
  float angle;
private:
  uint16_t colour;
  bool rendered;
};

Target targets[4];
void setup()
{
  randomSeed(analogRead(ANALOG_FLOAT));
  tft.init();

  //tft.setRotation(3);

  for (uint8_t i = 0; i < 4; ++i)
  {
    while (true)
    {
      const int8_t distance = random(32, 56);
      const float angle = random(0, 360) * pi() / 180.0f;
      const uint8_t x = 64 + sin(angle) * distance;
      const uint8_t y = 64 + cos(angle) * distance;

      bool good = true;
      for (uint8_t j = 0; j < i; ++j)
      {
        const Target& other = targets[j];
        const float distanceSq = static_cast<float>(other.x - x)*(other.x - x) + static_cast<float>(other.y - y)*(other.y - y);
        if (distanceSq < 128.0f)
        {
          good = false;
          break;
        }
      }
      if (!good)
        continue;

      targets[i] = Target(x, y, COLOURS[i], angle);
      break;
    }
  }
}

void renderBeam(const float angle, const bool clear = false)
{
  const float len = 60.0f;

  const float startAngle = angle - BEAM_WIDTH;
  const float endAngle = angle + BEAM_WIDTH;
  tft.drawLine(64, 64, static_cast<uint8_t>(64 + len * sin(startAngle)), static_cast<uint8_t>(64 + len * cos(startAngle)), !clear ? COLOUR_BEAM : 0x0000);
  tft.drawLine(64, 64, static_cast<uint8_t>(64 + len * sin(endAngle)), static_cast<uint8_t>(64 + len * cos(endAngle)), !clear ? COLOUR_BEAM : 0x0000);
}

void renderDot()
{
  tft.fillCircle(64, 64, 4, COLOUR_BEAM);
}

uint32_t last = 0;
bool active = true;
float lastAngle = 0.0f;
void loop()
{
  const uint32_t now = millis();
  const uint32_t delta = now - last;
  last = now;

  const float analogX = -(analogRead(ANALOG_X) / 8.0f - 64.0f);
  const float analogY = analogRead(ANALOG_Y) / 8.0f - 64.0f;
  const float len2 = analogX*analogX + analogY*analogY;


  if (len2 > 256.0f)
  {
    const float angle = atan2(analogY, analogX) - pi() / 2.0f;
    if (!active)
    {
      tft.fillCircle(64, 64, 4, colour(0.0f, 0.0f, 0.0f));
      active = true;
    }

    renderBeam(lastAngle, true);
    renderBeam(angle);
    lastAngle = angle;

    float beamLeft = angle - BEAM_WIDTH;
    float beamRight = angle + BEAM_WIDTH;
    while (beamLeft < 0.0f)
      beamLeft += pi() * 2.0f;
    while (beamLeft >= pi() * 2.0f)
      beamLeft -= pi() * 2.0f;
    while (beamRight < 0.0f)
      beamRight += pi() * 2.0f;
    while (beamRight >= pi() * 2.0f)
      beamRight -= pi() * 2.0f;
    for (uint8_t i = 0; i < 4; ++i)
    {
      const Target& target = targets[i];

      float angle = target.angle;
      while (angle < 0.0f)
        angle += pi() * 2.0f;
      while (angle >= pi() * 2.0f)
        angle -= pi() * 2.0f;

      if (target.inRange(beamLeft, beamRight))
        target.render();
      else
        target.clear();
    }
  }
  else if (active)
  {
    tft.fillScreen(colour(0.0f, 0.0f, 0.0f));
    renderDot();
    active = false;
  }
}

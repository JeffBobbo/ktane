// #define UTILITY_MODULE
// #include "shared/module.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <XTronical_ST7735.h>

#include "shared/debounce.h"

constexpr uint16_t colour(const float r, const float g, const float b);
constexpr uint16_t COLOUR_BLACK   = 0x0000;
constexpr uint16_t COLOUR_BLUE    = 0x001F;
constexpr uint16_t COLOUR_RED     = 0xF800;
constexpr uint16_t COLOUR_GREEN   = 0x07E0;
constexpr uint16_t COLOUR_CYAN    = 0x07FF;
constexpr uint16_t COLOUR_MAGENTA = 0xF81F;
constexpr uint16_t COLOUR_YELLOW  = 0xFFE0;
constexpr uint16_t COLOUR_WHITE   = 0xFFFF;

const uint8_t PIN_UP = 4;
const uint8_t PIN_DOWN = 5;
const uint8_t PIN_LEFT = 6;
const uint8_t PIN_RIGHT = 7;


Debounce up(PIN_UP);
Debounce down(PIN_DOWN);
Debounce left(PIN_LEFT);
Debounce right(PIN_RIGHT);


const uint8_t TFT_SLCK = 13;
const uint8_t TFT_MOSI = 11;
const uint8_t TFT_CS = 10;
const uint8_t TFT_RST = 9;
const uint8_t TFT_DC = 8;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);


class Option
{
public:
  Option() {}
  virtual String str() const = 0;
};

class OptionBool : public Option
{
public:
  OptionBool(const bool v) : value(v) {}
  virtual String str() const
  {
    return value ? 'True' : 'False';
  }

  const bool value;
};

class Setting
{
public:
  Setting(const String& n) : name(n), options(nullptr), selected(0) {}
  const String name;
  Option* options;
  Option* initial;
  size_t selected;
};

class Group
{
public:
  Group(const String& n) name(n), settings(nullptr), selected(0) {}
  const String name;
  Setting* settings;
  size_t selected;
};
Group* groups = nullptr;
size_t group = 0;

void render()
{
  tft.fillScreen(COLOUR_BLACK);

  int16_t x, y;
  uint16_t w, h;

  const Group& group = groups[group];
  tft.getTextBounds(group.name.c_str(), 0, 0, &x, &y, &w, &h);
  tft.setCursor((128 - w) / 2, 0);
  tft.print(group.name);

/*
  const Option* const option = &(menu.options[menu.index]);
  if (menu.active)
  {
    tft.getTextBounds(option->name.c_str(), 0, 0, &x, &y, &w, &h);
    tft.setCursor((128 - w) / 2, 0);
    tft.print(option->name.c_str());

    tft.setCursor(0, 40);
    // tft.print(option->min);
    tft.print("<=");
    // tft.print(option->value);
    tft.print("<=");
    tft.print(option->max);
    tft.setCursor(0, 48);
    tft.print("TEST");
  }
  else
  {
    tft.setTextSize(2);
    tft.getTextBounds("Menu", 0, 0, &x, &y, &w, &h);
    tft.setCursor((128 - w) / 2, 0);
    tft.print("Menu");

    if (option)
    {
      tft.getTextBounds(option->name.c_str(), 0, 0, &x, &y, &w, &h);
      tft.setCursor((128 - w) / 2, 40);
      tft.print(option->name.c_str());
    }
  }
  */
}


void setup()
{
  Setting relay = Setting("Clock Ticks");
  relay.options = malloc(2 * sizeof(OptionBool));
  relay.options[0] = OptionBool(false);
  relay.options[1] = OptionBool(true);
  relay.initial = new OptionBool(true);

  Group sound = Group('Sound');
  sound.options = malloc(1 * sizeof(Setting));
  sound.otions[0] = relay;

  groups = malloc(1 * sizeof(Group));
  groups[0] = sound;

  Serial.begin(9600);

  down.init();
  up.init();
  left.init();
  right.init();

  tft.init();
  tft.fillScreen(COLOUR_BLACK);
  tft.setRotation(2);

  render();
}


void loop()
{
  up.update();
  down.update();
  left.update();
  right.update();

/*
  if (right.is_released())
  {
    if (menu.active)
    {
      Option& option = menu.options[menu.index];
      if (option.value < option.max)
        ++option.value;
    }
    else
    {
      menu.index = (menu.index + NUM_OPTIONS + 1) % NUM_OPTIONS;
    }
    render();
  }
  else if (left.is_released())
  {
    if (menu.active)
    {
      Option& option = menu.options[menu.index];
      if (option.value > option.min)
        --option.value;
    }
    else
    {
      menu.index = (menu.index + NUM_OPTIONS - 1) % NUM_OPTIONS;
    }
    render();
  }
  else if (up.is_released())
  {
    menu.active = !menu.active;
    render();
  }
  */
}

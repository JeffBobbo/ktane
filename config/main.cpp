#define UTILITY_MODULE
#include "shared/module.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "shared/debounce.h"


const uint8_t PIN_UP = 4;
const uint8_t PIN_DOWN = 5;
const uint8_t PIN_LEFT = 6;
const uint8_t PIN_RIGHT = 7;
Debounce up(PIN_UP);
Debounce down(PIN_DOWN);
Debounce left(PIN_LEFT);
Debounce right(PIN_RIGHT);

Adafruit_SSD1306 display(128, 64);

struct Option {
  String name;
  int32_t value;
  int32_t min;
  int32_t max;
};
Option opt_strikes = {
  String("Strikes"),
  3,
  1,
  9
};
Option opt_time = {
  String("Time"),
  3,
  1,
  15
};
Option opt_volume = {
  String("Volume"),
  4,
  0,
  4
};
Option opt_relay = {
  String("Clock Ticker"),
  1,
  0,
  1
};
Option opt_hardmode = {
  String("Hard Mode"),
  0,
  0,
  1
};

struct Menu {
  Option options[5];
  int32_t index;
  bool active;
};
Menu menu = {
  {opt_strikes, opt_time, opt_volume, opt_relay, opt_hardmode},
  0,
  false
};
const int32_t NUM_OPTIONS = sizeof(menu.options) / sizeof(Option);


void render()
{
  display.clearDisplay();

  int16_t x, y;
  uint16_t w, h;
  Option* option = menu.index >= 0 ? &(menu.options[menu.index]) : nullptr;

  if (menu.active)
  {
    if (option)
    {
      display.setTextSize(2);
      display.getTextBounds(option->name.c_str(), 0, 0, &x, &y, &w, &h);
      display.setCursor((128 - w) / 2, 0);
      display.print(option->name.c_str());

      char value[4];
      itoa(option->value, value, 10);
      char max[4];
      itoa(option->max, max, 10);

      display.setCursor(0, 40);
      display.print(option->min);
      
      display.getTextBounds(max, 0, 0, &x, &y, &w, &h);
      display.setCursor(128 - w, 40);
      display.print(max);

      display.getTextBounds(value, 0, 0, &x, &y, &w, &h);
      display.setCursor((128 - w) / 2, 40);
      display.print(value);
    }
  }
  else
  {
    display.setTextSize(2);
    display.getTextBounds("Menu", 0, 0, &x, &y, &w, &h);
    display.setCursor((128 - w) / 2, 0);
    display.print("Menu");

    if (option)
    {
      display.getTextBounds(option->name.c_str(), 0, 0, &x, &y, &w, &h);
      display.setCursor((128 - w) / 2, 40);
      display.print(option->name.c_str());
    }
  }

  display.display();
}

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  down.init();
  up.init();
  left.init();
  right.init();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    while (true)
    {
      digitalWrite(LED_BUILTIN, 1);
      delay(250);
      digitalWrite(LED_BUILTIN, 0);
      delay(250);
    }
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextWrap(false);
  display.dim(1);
  display.display();
}

void loop()
{
  up.update();
  down.update();
  left.update();
  right.update();

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
  }
  else if (up.is_released())
    menu.active = !menu.active;

  render();
}

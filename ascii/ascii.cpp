#include "shared/module.h"
#include "shared/address.h"
#include "shared/debounce.h"

#include <LiquidCrystal.h>

const Address addr = address::ASCII;
const uint8_t PIN_DISARM_LED = 5;

const uint8_t PIN_ENTER = 4;
const uint8_t PIN_SWITCHES[] = {6, 7, 8, 9, 10, 11, 12, 13};
const size_t NUM_SWITCHES = sizeof(PIN_SWITCHES);

// LCD pins
const uint8_t PIN_RS = 2;
const uint8_t PIN_ENABLE = 3;
const uint8_t PIN_D4 = A3;
const uint8_t PIN_D5 = A2;
const uint8_t PIN_D6 = A1;
const uint8_t PIN_D7 = A0;

const uint8_t PIN_SEED = A7; // unused until on Nano?

Debounce enter(PIN_ENTER);

const size_t LCD_WIDTH = 16;
LiquidCrystal lcd(PIN_RS, PIN_ENABLE, PIN_D4, PIN_D5, PIN_D6, PIN_D7);


char* code = "AAAA";
size_t length = strlen(code);

size_t progress;
uint8_t last;
bool guessed;

void initialise()
{
  randomSeed(analogRead(PIN_SEED));

  enter.init();
  for (size_t i = 0; i < sizeof(PIN_SWITCHES); ++i)
    pinMode(PIN_SWITCHES[i], INPUT);

  lcd.begin(LCD_WIDTH, 2);
  reset();
}

void reset()
{
  for (size_t i = 0; i < length; ++i)
  {
    char c = random('!', '~'); // exclamation to TILDE exclusive
    while (c == '\\' || (i > 0 && c == code[i-1]))
      c = random('!', '~');
    code[i] = c;
  }

  lcd.clear();
  lcd.noDisplay();
  progress = 0;
  last = 255;
  guessed = false;

  state = ModuleState::READY;
}

void onIndicators()
{
}

void arm()
{
  lcd.display();
  lcd.setCursor((LCD_WIDTH - length) / 2, 0);
  lcd.print(code);
  lcd.setCursor((LCD_WIDTH - length) / 2, 1);
  lcd.blink();
}

void stop()
{
  lcd.noBlink();
}

void idle()
{
  enter.update();

  if (enter.is_released())
  {
    uint8_t value = 0x0;
    for (size_t i = 0; i < sizeof(PIN_SWITCHES); ++i)
      value = (value >> 1) | (digitalRead(PIN_SWITCHES[i]) << (NUM_SWITCHES - 1));

    if (guessed && value == last)
      return;
    guessed = true;
    last = value;
    if (indicators.strikes && static_cast<uint8_t>(~value) == code[progress] || !indicators.strikes && value == code[progress])
    {
      lcd.write(code[progress]);
      ++progress;
      if (progress == length)
      {
        lcd.noBlink();
        disarm();
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
  stop();
}

void detonate()
{
  stop();
}

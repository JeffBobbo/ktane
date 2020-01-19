#define UTILITY_MODULE
#include "shared/module.h"
#include "shared/debounce.h"

const Address addr = address::FIZZBUZZ;

const uint8_t PIN_NEITHER = 7;
const uint8_t PIN_FIZZ = 6;
const uint8_t PIN_BUZZ = 5;
const uint8_t PIN_FIZZBUZZ = 4;

const uint8_t PIN_DATA = 8;
const uint8_t PIN_CLOCK = 9;
const uint8_t PIN_LATCH = 10;

const uint8_t PIN_RANDOM_SEED = A0;

const uint8_t SEG_TABLE[] = {
  B11000000, //0
  B11111001, //1
  B10100100, //2
  B10110000, //3
  B10011001, //4
  B10010010, //5
  B10000010, //6
  B11111000, //7
  B10000000, //8
  B10011000 //9
};

void segment(const uint8_t v)
{
  digitalWrite(PIN_LATCH, 0);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, v == 255 ? 0 : SEG_TABLE[v % 10]);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, v == 255 ? 0 : SEG_TABLE[(v / 10) % 10]);
  digitalWrite(PIN_LATCH, 1);
}

Debounce neither(PIN_NEITHER);
Debounce fizz(PIN_FIZZ);
Debounce buzz(PIN_BUZZ);
Debounce fizzbuzz(PIN_FIZZBUZZ);

uint8_t number;
int32_t start = 0;

const int32_t TIME_ALLOWED = 10000;

enum class Choice {
  NONE,
  NEITHER,
  FIZZ,
  BUZZ,
  FIZZBUZZ
};

Choice choice;

void initialise()
{
  neither.init();
  fizz.init();
  buzz.init();
  fizzbuzz.init();

  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);

  randomSeed(analogRead(PIN_RANDOM_SEED));
  reset();
}

void reset()
{
  state = ModuleState::READY;
}

void onIndicators()
{
}

void arm()
{
  segment(255);
  number = random(1, 100);
  choice = Choice::NONE;
  start = 5*1000;//random(30, 60) * 1000;
}

bool test()
{
  const bool fizz = number % 3 == 0;
  const bool buzz = number % 5 == 0;

  switch (choice)
  {
    case Choice::NONE:
      return false;
    case Choice::NEITHER:
      return !fizz && !buzz;
    case Choice::FIZZ:
      return fizz && !buzz;
    case Choice::BUZZ:
      return !fizz && buzz;
    case Choice::FIZZBUZZ:
      return fizz && buzz;
  }
  return false;
}

void idle()
{
  neither.update();
  fizz.update();
  buzz.update();
  fizzbuzz.update();

  const int32_t now = millis();
  if (neither.is_released())
    choice = Choice::NEITHER;
  else if (fizz.is_released())
    choice = Choice::FIZZ;
  else if (buzz.is_released())
    choice = Choice::BUZZ;
  else if (fizzbuzz.is_released())
    choice = Choice::FIZZBUZZ;

  if (now > start + TIME_ALLOWED)
  {
    segment(number);
    if (choice != Choice::NONE)
    {
      if (!test())
        strike();
      arm();
    }
  }

}

void defuse()
{
}

void detonate()
{
}

#define UTILITY_MODULE
#include "shared/module.h"
#include "shared/debounce.h"

const Address addr = address::FIZZBUZZ;

const uint8_t PIN_NEITHER = 5;
const uint8_t PIN_FIZZ = 4;
const uint8_t PIN_BUZZ = 3;
const uint8_t PIN_FIZZBUZZ = 2;

const uint8_t PIN_DATA = 6;
const uint8_t PIN_CLOCK = 7;
const uint8_t PIN_LATCH = 8;

const uint8_t PIN_GREEN = 10;
const uint8_t PIN_RED = 11;

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
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, v == 255 ? 255 : SEG_TABLE[v % 10]);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, v == 255 ? 255 : SEG_TABLE[(v / 10) % 10]);
  digitalWrite(PIN_LATCH, 1);
}

Debounce neither(PIN_NEITHER);
Debounce fizz(PIN_FIZZ);
Debounce buzz(PIN_BUZZ);
Debounce fizzbuzz(PIN_FIZZBUZZ);

uint8_t number;
int32_t start = 0;

const int32_t TIME_ALLOWED = 30000;

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
  analogWrite(PIN_RED, 0);
  analogWrite(PIN_GREEN, 0);
  segment(255);

  state = ModuleState::READY;
}

void onIndicators()
{
}

void arm()
{
  segment(255);
  switch (random(0, 3))
  {
    case 0:
      number = random(1, 100); // 1 .. 99
      break;
    case 1:
      number = random(1, 33) * 3; // 3 .. 99
      break;
    case 2:
      number = random(1, 20) * 5; // 5 .. 95
      break;
    case 3:
      number = random(1, 7) * 15; // 15 .. 90
      break;
  }
  choice = Choice::NONE;
  start = millis() + random(60, 75) * 1000;

  digitalWrite(PIN_RED, 0);
  digitalWrite(PIN_GREEN, 1);
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

  if (now > start)
  {
    digitalWrite(PIN_GREEN, 0);
    segment(number);
    const int32_t remain = start + TIME_ALLOWED - now;
    digitalWrite(PIN_RED, static_cast<int32_t>(remain / sqrt(remain) * 0.5f) % 2);

    if (choice != Choice::NONE)
    {
      if (!test())
        strike();
      arm();
    }
    if (now > start + TIME_ALLOWED)
    {
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

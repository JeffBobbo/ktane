#define UTILITY_MODULE
#include "shared/module.h"

const Address addr = address::INDICATORS;

const uint8_t PIN_STRIKE_0 = 2;
const uint8_t PIN_STRIKE_1 = 3;
const uint8_t PIN_STRIKE_2 = 4;

const uint8_t PIN_DATA  = 5;
const uint8_t PIN_CLOCK = 6;
const uint8_t PIN_LATCH = 7;

const uint8_t PIN_DEFUSED = 8;

const uint8_t PIN_BINARY_0 = 9;
const uint8_t PIN_BINARY_1 = 10;
const uint8_t PIN_BINARY_2 = 11;
const uint8_t PIN_BINARY_3 = 12;

const uint8_t PIN_EXPLODED = 13;

//const uint8_t SEG_TABLE[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
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

// void segment(const uint8_t v)
// {
//   digitalWrite(PIN_LATCH, 0);
//   shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, v == 255 ? 0 : SEG_TABLE[v & 0x0F]);
//   digitalWrite(PIN_LATCH, 1);
// }
void segment(const uint8_t v)
{
  digitalWrite(PIN_LATCH, 0);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, v == 255 ? 0 : SEG_TABLE[v % 10]);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, v == 255 ? 0 : SEG_TABLE[(v / 10) % 10]);
  digitalWrite(PIN_LATCH, 1);
}

void display(const Indicators& indicators)
{
  digitalWrite(PIN_STRIKE_0, indicators.strikes > 0);
  digitalWrite(PIN_STRIKE_1, indicators.strikes > 1);
  digitalWrite(PIN_STRIKE_2, indicators.strikes > 2);

  digitalWrite(PIN_BINARY_0, indicators.binary & 0b0001);
  digitalWrite(PIN_BINARY_1, indicators.binary & 0b0010);
  digitalWrite(PIN_BINARY_2, indicators.binary & 0b0100);
  digitalWrite(PIN_BINARY_3, indicators.binary & 0b1000);

  segment(indicators.numerical);
}

void initialise()
{
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);

  pinMode(PIN_STRIKE_0, OUTPUT);
  pinMode(PIN_STRIKE_1, OUTPUT);
  pinMode(PIN_STRIKE_2, OUTPUT);

  pinMode(PIN_BINARY_0, OUTPUT);
  pinMode(PIN_BINARY_1, OUTPUT);
  pinMode(PIN_BINARY_2, OUTPUT);
  pinMode(PIN_BINARY_3, OUTPUT);

  pinMode(PIN_DEFUSED, OUTPUT);
  pinMode(PIN_EXPLODED, OUTPUT);

  reset();
}

void reset()
{
  digitalWrite(PIN_STRIKE_0, 0);
  digitalWrite(PIN_STRIKE_1, 0);
  digitalWrite(PIN_STRIKE_2, 0);

  digitalWrite(PIN_BINARY_0, 0);
  digitalWrite(PIN_BINARY_1, 0);
  digitalWrite(PIN_BINARY_2, 0);
  digitalWrite(PIN_BINARY_3, 0);

  segment(255);

  digitalWrite(PIN_DEFUSED, 0);
  digitalWrite(PIN_EXPLODED, 0);
}

void onIndicators()
{
  display(indicators);
}

void arm()
{
}

void idle()
{
}

void defuse()
{
  digitalWrite(PIN_DEFUSED, 1);
}

void detonate()
{
  digitalWrite(PIN_EXPLODED, 1);
}

#include "shared/module.h"
#include "shared/util.h"
#include "shared/debounce.h"

#include <Encoder.h>

const Address addr = address::SAFE;
const uint8_t PIN_DISARM_LED = 8;

const int16_t NUM_STEPS = 100;

// encoder
const uint8_t PIN_CLK = 2;
const uint8_t PIN_DT = 3;
const uint8_t PIN_BUTTON = 4;

// display
const uint8_t PIN_DATA = 5;
const uint8_t PIN_LATCH = 6;
const uint8_t PIN_CLOCK = 7;

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

const uint8_t CODE_LENGTH = 3;
int16_t code[CODE_LENGTH] = {0};

bool calibrated;

Encoder encoder(PIN_CLK, PIN_DT);

uint8_t index;

Debounce button(PIN_BUTTON, true);
void initialise()
{
  pinMode(PIN_DT, INPUT);
  pinMode(PIN_CLK, INPUT);

  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);

  digitalWrite(button.getPin(), 1); // turn on internal pullup

  reset();
}

void reset()
{
  segment(255);
  index = 0;
}

void onIndicators()
{
  const uint8_t digits = indicators.numerical % 10;
  const uint8_t tens = (indicators.numerical / 10) % 10;
  const uint8_t a = (NUM_STEPS - 1) - indicators.numerical;
  const uint8_t b = indicators.numerical;
  const uint8_t c = digits * 10 + tens;


  // abc, bac, acb, bca, cba, cab
  switch (indicators.strikes)
  {
    case 0:
      code[0] = a;
      code[1] = b;
      code[2] = c;
      break;
    case 1:
      code[0] = b;
      code[1] = a;
      code[2] = c;
      break;
    case 2:
      code[0] = a;
      code[1] = c;
      code[2] = b;
      break;
    case 3:
      code[0] = b;
      code[1] = c;
      code[2] = c;
      break;
    case 4:
      code[0] = c;
      code[1] = b;
      code[2] = a;
      break;
    default:
      code[0] = c;
      code[1] = a;
      code[2] = b;
      break;
  }

  if (state == ModuleState::INITIALISATION)
    state = ModuleState::READY;
}

void arm()
{
}

void idle()
{
  button.update();

  const int16_t value = (encoder.read() + NUM_STEPS * 4) % (NUM_STEPS * 4);
  encoder.write(value);
  segment(value / 4);

  if (button.is_released())
  {
    if (code[index] != value / 4)
    {
      strike();
    }
    else
    {
      ++index;
      if (index >= CODE_LENGTH)
        disarm();
    }
  }
}

void defuse()
{
}

void detonate()
{
}

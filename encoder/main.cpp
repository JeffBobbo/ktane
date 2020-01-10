#include "shared/module.h"
#include "shared/util.h"

#include <Encoder.h>

const Address addr = address::SAFE;
const uint8_t PIN_DISARM_LED = 8;

const uint8_t NUM_STEPS = 20;

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
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, v == 255 ? 0 : SEG_TABLE[v % 10]);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, v == 255 ? 0 : SEG_TABLE[(v / 10) % 10]);
  digitalWrite(PIN_LATCH, 1);
}


class Debounce
{
public:
  Debounce(const uint8_t p, const bool low = false) : pin(p), active_low(low), state(0)
  {
    pinMode(pin, INPUT);
  }

  inline uint8_t getPin() const { return pin; }

  void update() {
    state = state << 1 | (active_low ? !digitalRead(pin) : digitalRead(pin));
  }

  bool is_pressed()
  {
    const uint8_t MASK = 0b11000111;
    if ((state & MASK) == 0b00000111)
    {
      state = 0b11111111;
      return true;
    }
    return false;
  }

  bool is_released()
  {
    const uint8_t MASK = 0b11000111;
    if ((state & MASK) == 0b11000000)
    {
      state = 0b00000000;
      return true;
    }
    return false;
  }

  uint8_t is_down() const
  {
    return state == 0b11111111;
  }
  uint8_t is_up() const
  {
    return state == 0b00000000;
  }

private:
  const uint8_t pin;
  const bool active_low;
  uint8_t state;
};




const uint8_t CODE_LENGTH = 3;
int8_t code[CODE_LENGTH] = {0};

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
  Serial.begin(9600);
}

void reset()
{
  index = 0;
}

void onIndicators()
{
  if (state == ModuleState::INITIALISATION)
  {
    code[0] = (util::countEvens(indicators.serial) + util::countOdds(indicators.serial)) % NUM_STEPS;
    code[1] = (code[0] - (util::hasEvens(indicators.serial) ? 10 : 5) + NUM_STEPS) % NUM_STEPS;
    code[2] = (code[1] + (util::hasVowels(indicators.serial) ? 7 : 13) + NUM_STEPS) % NUM_STEPS;
    state = ModuleState::READY;
  }
}

void arm()
{
}

void idle()
{
  button.update();

  const int8_t value = (encoder.read() + NUM_STEPS * 4) % (NUM_STEPS * 4);
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

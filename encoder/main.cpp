#include <Arduino.h>
#include <Wire.h>

#include "shared/module.h"
#include "shared/util.h"

#include <Encoder.h>

const Address addr = address::SAFE;
const uint8_t PIN_DISARM_LED = 5;

const uint8_t NUM_STEPS = 20;

// encoder
const uint8_t PIN_CLK = 2;
const uint8_t PIN_DT = 3;
const uint8_t PIN_BUTTON = 4;

const uint8_t CODE_LENGTH = 3;
int8_t code[CODE_LENGTH] = {0};

bool calibrated;

Encoder encoder(PIN_CLK, PIN_DT);

uint8_t index;

void initialise()
{
  pinMode(PIN_DT, INPUT);
  pinMode(PIN_CLK, INPUT);
  pinMode(PIN_BUTTON, INPUT);

  digitalWrite(PIN_BUTTON, 1); // turn on internal pullup

  reset();
}

void reset()
{
  index = 0;
  calibrated = false;
}

void onIndicators()
{
  if (state == ModuleState::INITIALISATION)
  {
    code[0] = util::countEvens(indicators.serial) + util::countOdds(indicators.serial);
    code[1] = code[0] - (util::hasEvens(indicators.serial) ? 10 : 5);
    code[2] = code[1] + (util::hasVowels(indicators.serial) ? 7 : 13);
    state = ModuleState::READY;
  }
}

void arm()
{
}

void idle()
{
  if (!digitalRead(PIN_BUTTON))
  {
    while (!digitalRead(PIN_BUTTON));
    if (calibrated)
    {
      Serial.print(encoder.read() / 4);
      Serial.print(" == ");
      Serial.println(code[index]);
      if (code[index] != encoder.read() / 4)
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
    else
    {
      encoder.write(0);
      calibrated = true;
    }
  }
}

void defuse()
{
}

void detonate()
{
}

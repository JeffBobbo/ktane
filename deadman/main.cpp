#define UTILITY_MODULE
#include "shared/module.h"

const Address addr = address::DEADMAN;

// outputs
const uint8_t PIN_LED = 4;
const uint8_t PIN_COOLDOWN = 5;

// inputs
const uint8_t PIN_SWITCH = 2;

// how long allowed to not die
const uint32_t DETONATION_TIME = 15000;

// cooldown between strikes
const uint32_t COOLDOWN = 15000;

// time when button was last released (or 0 for held)
uint32_t released;

void initialise()
{
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_COOLDOWN, OUTPUT);
  pinMode(PIN_SWITCH, INPUT);
}

void reset()
{
  // setup pins
  digitalWrite(PIN_LED, 0);
}

void onIndicators()
{
}

void arm()
{
  released = millis() + COOLDOWN;
}

void idle()
{
  if (digitalRead(PIN_SWITCH))
  {
    if (released)
    {
      released = 0;
      digitalWrite(PIN_LED, 0);
    }
  }
  else
  {
    if (!released)
      released = millis();
  }

  if (released)
  {
    if (millis() > released + DETONATION_TIME)
    {
      strike();
      released = millis() + COOLDOWN;
    }
    else
    {
      const uint32_t remain = released + DETONATION_TIME - millis();
      if (remain > DETONATION_TIME)
      {
        digitalWrite(PIN_COOLDOWN, 1);
      }
      else
      {
        digitalWrite(PIN_COOLDOWN, 0);
        digitalWrite(PIN_LED, static_cast<uint32_t>(remain / sqrt(remain) * 0.5f) % 2);
      }
    }
  }
}

void defuse()
{
}

void detonate()
{
}

#include <Arduino.h>
#include <Wire.h>

#include "shared/address.h"
#include "shared/message.h"

// outputs
const uint8_t PIN_LED = 5;
const uint8_t PIN_STRIKE = 4;

// inputs
const uint8_t PIN_SWITCH = 9;

// how long allowed to not die
const uint32_t DETONATION_TIME = 5000;

// cooldown between strikes
const uint32_t COOLDOWN = 15000;

void setup()
{
  // setup pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_STRIKE, OUTPUT);
  digitalWrite(PIN_LED, 0);
  digitalWrite(PIN_STRIKE, 0);

  pinMode(PIN_SWITCH, INPUT);
}

uint32_t released = 0;
void loop()
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
      digitalWrite(PIN_STRIKE, 1);
      delay(500);
      digitalWrite(PIN_STRIKE, 0);
      delay(500);
      released = millis() + COOLDOWN;
    }
    else
    {
      const uint32_t remain = released + DETONATION_TIME - millis();
      digitalWrite(PIN_LED, static_cast<uint32_t>(remain / sqrt(remain) * 0.5f) % 2);
    }
  }
}

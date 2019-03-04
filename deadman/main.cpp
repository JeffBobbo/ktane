#include <Arduino.h>
#include <Wire.h>

#include "shared/address.h"
#include "shared/message.h"

// outputs
const uint8_t PIN_LED = 4;
const uint8_t PIN_COOLDOWN = 5;

// inputs
const uint8_t PIN_SWITCH = 2;

// how long allowed to not die
const uint32_t DETONATION_TIME = 5000;

// cooldown between strikes
const uint32_t COOLDOWN = 15000;

// time when button was last released (or 0 for held)
uint32_t released;

Status status;

void reset()
{
  status.state = ModuleState::READY;
  status.strikes = 0;

  // released is set when we transition from READY to ARMED
}

void receiveEvent(int count)
{
  if (count == 0)
    return;

  Message msg;
  for (int i = 0; i < count; ++i)
    reinterpret_cast<uint8_t*>(&msg)[i] = Wire.read();

  switch (msg.opcode)
  {
    case OpCode::ARM:
      status.state = ModuleState::ARMED;
    break;
    case OpCode::DEFUSED:
    case OpCode::EXPLODED:
      status.state = ModuleState::STOP;
    break;
    case OpCode::RESET:
      reset();
    break;
    default:
    break;
  }
}

void requestEvent()
{
  Wire.write(reinterpret_cast<uint8_t*>(&status), sizeof(status));
  status.strikes = 0;
}

void setup()
{
  // setup pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_COOLDOWN, OUTPUT);
  digitalWrite(PIN_LED, 0);

  pinMode(PIN_SWITCH, INPUT);

  Wire.begin(address::DEADMAN);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  reset();
}

void loop()
{
  if (status.state != ModuleState::ARMED)
  {
    released = millis() + COOLDOWN;
    return;
  }

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
      status.strikes = 1;
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

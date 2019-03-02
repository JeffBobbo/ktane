#include <Arduino.h>
#include <Wire.h>

#include "shared/address.h"
#include "shared/message.h"

// outputs
const uint8_t PIN_LED = 5;

// inputs
const uint8_t PIN_SWITCH = 9;

// how long allowed to not die
const uint32_t DETONATION_TIME = 5000;

// cooldown between strikes
const uint32_t COOLDOWN = 15000;

// time when button was last released (or 0 for held)
uint32_t released = 0;

Status status = {ModuleState::INITIALISATION, 0};

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
  status.state = ModuleState::READY;

  // setup pins
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, 0);

  pinMode(PIN_SWITCH, INPUT);

  Wire.begin(address::DEADMAN);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop()
{
  if (status.state != ModuleState::ARMED)
    return;

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
      digitalWrite(PIN_LED, static_cast<uint32_t>(remain / sqrt(remain) * 0.5f) % 2);
    }
  }
}

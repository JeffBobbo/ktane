#include <Arduino.h>
#include <Wire.h>

#include "shared/address.h"
#include "shared/message.h"

#include <Encoder.h>

const uint8_t NUM_STEPS = 20;

// encoder
const uint8_t PIN_CLK = 2;
const uint8_t PIN_DT = 3;
const uint8_t PIN_BUTTON = 10;

// defuse status
const uint8_t PIN_DISARMED_LED = 4;

const uint8_t CODE_LENGTH = 3;
int8_t code[CODE_LENGTH] = {0};

bool calibrated = false;

Encoder encoder(2, 3);

Status status = {ModuleState::INITIALISATION, 0};

uint8_t index = 0;

void receiveEvent(int count)
{
  Message msg;

  for (int i = 0; i < count; ++i)
    reinterpret_cast<uint8_t*>(&msg)[i] = Wire.read();

  // process message
  switch (msg.opcode)
  {
    case OpCode::ARM:
      status.state = ModuleState::ARMED;
    break;
  }
}

void requestEvent()
{
  Wire.write(reinterpret_cast<uint8_t*>(&status), sizeof(status));

  // reset this to zero once it's been sent
  status.strikes = 0;
}

void setup()
{
  status.state = ModuleState::INITIALISATION;

  pinMode(PIN_DT, INPUT);
  pinMode(PIN_CLK, INPUT);
  pinMode(PIN_BUTTON, INPUT);

  digitalWrite(PIN_BUTTON, 1); // turn on internal pullup

  code[0] = 1;//version_sum() % 20;
  code[1] = 1;//code[0] - (version_hasEven() ? 10 : 5);
  code[2] = 1;//code[1] + (version_hasVowel() ? 7 : 13);

  Wire.begin(address::SAFE);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
  status.state = ModuleState::READY;
}

void loop()
{
  // don't do anything until we're ready
  if (status.state != ModuleState::ARMED)
    return;

  digitalWrite(PIN_DISARMED_LED, 1);

  if (!digitalRead(PIN_BUTTON))
  {
    while (!digitalRead(PIN_BUTTON));
    if (calibrated)
    {
      if (code[index] != encoder.read() / 4)
      {
        ++status.strikes;
      }
      else
      {
        ++index;
        if (index >= CODE_LENGTH)
        {
          status.state = ModuleState::DISARMED;
          digitalWrite(PIN_DISARMED_LED, 0);
        }
      }
    }
    else
    {
      encoder.write(0);
      calibrated = true;
    }
  }
}

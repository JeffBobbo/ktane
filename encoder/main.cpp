#include <Arduino.h>
#include <Wire.h>

#include "shared/address.h"
#include "shared/config.h"
#include "shared/message.h"
#include "shared/util.h"

#include <Encoder.h>

const uint8_t NUM_STEPS = 20;

// encoder
const uint8_t PIN_CLK = 2;
const uint8_t PIN_DT = 3;
const uint8_t PIN_BUTTON = 5;

// defuse status
const uint8_t PIN_DISARMED_LED = 4;

const uint8_t CODE_LENGTH = 3;
int8_t code[CODE_LENGTH] = {0};

char version[VERSION_LENGTH+1] = {0};
bool calibrated;

Encoder encoder(PIN_CLK, PIN_DT);

Status status;

uint8_t index;

void reset()
{
  index = 0;
  calibrated = false;
  if (status.state != ModuleState::READY)
    status.state = ModuleState::INITIALISATION;
  status.strikes = 0;
}

void receiveEvent(int count)
{
  // if count is 0, this is an ACK from master to check if we're alive
  if (count == 0)
    return;

  Message msg;

  for (int i = 0; i < count; ++i)
    reinterpret_cast<uint8_t*>(&msg)[i] = Wire.read();

  // process message
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
    case OpCode::VERSION:
      strncpy(version, reinterpret_cast<char*>(msg.data), VERSION_LENGTH+1);
      code[0] = util::countEvens(version) + util::countOdds(version);
      code[1] = code[0] - (util::hasEvens(version) ? 10 : 5);
      code[2] = code[1] + (util::hasVowels(version) ? 7 : 13);
      status.state = ModuleState::READY;
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
  pinMode(PIN_DT, INPUT);
  pinMode(PIN_CLK, INPUT);
  pinMode(PIN_BUTTON, INPUT);

  digitalWrite(PIN_BUTTON, 1); // turn on internal pullup

  Wire.begin(address::SAFE);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  reset();
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
        status.strikes = 1;
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

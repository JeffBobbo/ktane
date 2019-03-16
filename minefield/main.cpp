#include <Arduino.h>
#include <Wire.h>

#include <LedControl.h>

#include "shared/address.h"
#include "shared/message.h"

#include "paths.h"

const uint8_t PIN_NORTH = 7;
const uint8_t PIN_EAST = 6;
const uint8_t PIN_SOUTH = 5;
const uint8_t PIN_WEST = 4;

const uint8_t PIN_SEED = A0;

const uint8_t PIN_DISARMED_LED = 8;

LedControl display = LedControl(12, 11, 10, 1);

uint8_t progress;

Path path;

Status status;

void reset()
{
  display.clearDisplay(0);

  progress = 0;
  path = paths[random(NUM_PATHS)];

  status.state = ModuleState::READY;
  status.strikes = 0;
  digitalWrite(PIN_DISARMED_LED, 1);
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
  randomSeed(analogRead(PIN_SEED));

  // setup display
  display.setIntensity(0, 1);
  display.clearDisplay(0);
  display.shutdown(0, false);

  pinMode(PIN_NORTH, INPUT);
  pinMode(PIN_EAST, INPUT);
  pinMode(PIN_SOUTH, INPUT);
  pinMode(PIN_WEST, INPUT);

  pinMode(PIN_DISARMED_LED, OUTPUT);

  Wire.begin(address::MINEFIELD);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  reset();
}

void displayCell() {
  display.setLed(0, path.path[progress] / 8, path.path[progress] % 8, true);
}

void displayMaze() {
  display.clearDisplay(0);
  for (uint8_t i = 0; i <= progress; ++i)
    display.setLed(0, path.path[i] / 8, path.path[i] % 8, true);
}

uint8_t lastProgress = 255;
void loop()
{
  if (status.state != ModuleState::ARMED)
    return;

  uint8_t cell;
  bool pressed = false;

  if (digitalRead(PIN_NORTH))
  {
    while (digitalRead(PIN_NORTH));
    cell = path.path[progress] + 8;
    pressed = true;
  }
  else if (digitalRead(PIN_EAST))
  {
    while (digitalRead(PIN_EAST));
    cell = path.path[progress] + 1;
    pressed = true;
  }
  else if (digitalRead(PIN_SOUTH))
  {
    while (digitalRead(PIN_SOUTH));
    cell = path.path[progress] - 8;
    pressed = true;
  }
  else if (digitalRead(PIN_WEST))
  {
    while (digitalRead(PIN_WEST));
    cell = path.path[progress] - 1;
    pressed = true;
  }
  if (pressed)
  {
    if (cell == path.path[progress+1])
    {
      if (++progress == path.length - 1)
      {
        status.state = ModuleState::DISARMED;
        digitalWrite(PIN_DISARMED_LED, 0);
      }
    }
    else
    {
      status.strikes = 1;
    }
  }

  if (progress != lastProgress)
  {
    displayCell();
    lastProgress = progress;
  }
}

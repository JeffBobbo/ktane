#include "shared/module.h"

#include <LedControl.h>

#include "paths.h"

const Address addr = address::MINEFIELD;
const uint8_t PIN_DISARM_LED = 8;

const uint8_t PIN_NORTH = 7;
const uint8_t PIN_EAST = 6;
const uint8_t PIN_SOUTH = 5;
const uint8_t PIN_WEST = 4;

const uint8_t PIN_SEED = A0;


LedControl display = LedControl(12, 11, 10, 1);

uint8_t progress;

Path path;

void initialise()
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
}

void reset()
{
  display.clearDisplay(0);

  progress = 0;
  path = paths[random(NUM_PATHS)];
}

void onIndicators()
{
}

void arm()
{
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
void idle()
{
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
        disarm();
    }
    else
    {
      strike();
    }
  }

  if (progress != lastProgress)
  {
    displayCell();
    lastProgress = progress;
  }
}

void detonate()
{
}

void defuse()
{
}

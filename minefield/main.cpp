#include "shared/module.h"
#include "shared/debounce.h"

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

size_t progress;

Path path;

Debounce north(PIN_NORTH);
Debounce east(PIN_EAST);
Debounce south(PIN_SOUTH);
Debounce west(PIN_WEST);

void initialise()
{
  randomSeed(analogRead(PIN_SEED));

  // setup display
  display.setIntensity(0, 1);
  display.clearDisplay(0);
  display.shutdown(0, false);

  reset();
}

void reset()
{
  display.clearDisplay(0);

  progress = 0;
  path = paths[random(NUM_PATHS)];
  state = ModuleState::READY;
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
  for (size_t i = 0; i <= progress; ++i)
    display.setLed(0, path.path[i] / 8, path.path[i] % 8, true);
}

size_t lastProgress = 255;
void idle()
{
  size_t cell;
  bool pressed = false;

  north.update();
  east.update();
  south.update();
  west.update();

  if (north.is_released())
  {
    cell = path.path[progress] + 8;
    pressed = true;
  }
  else if (east.is_released())
  {
    cell = path.path[progress] + 1;
    pressed = true;
  }
  else if (south.is_released())
  {
    cell = path.path[progress] - 8;
    pressed = true;
  }
  else if (west.is_released())
  {
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

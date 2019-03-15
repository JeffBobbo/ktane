#include <Arduino.h>

#include <LedControl.h>

#include "paths.h"

const uint8_t PIN_NORTH = 7;
const uint8_t PIN_EAST = 6;
const uint8_t PIN_SOUTH = 5;
const uint8_t PIN_WEST = 4;

const uint8_t PIN_SEED = A0;

const uint8_t PIN_DISARM_LED = 8;

LedControl display = LedControl(12, 11, 10, 1);

uint8_t progress = 0;

Path path;

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

  pinMode(PIN_DISARM_LED, OUTPUT);
  digitalWrite(PIN_DISARM_LED, 0);

  path = paths[random(NUM_PATHS)];
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
  uint8_t cell;
  bool pressed = false;
  if (progress == path.length - 1)
    return;

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
      ++progress;
    else
    {
      digitalWrite(PIN_DISARM_LED, 1);
      delay(500);
      digitalWrite(PIN_DISARM_LED, 0);
      delay(500);
    }
  }

  if (progress != lastProgress)
  {
    displayCell();
    lastProgress = progress;
  }
}

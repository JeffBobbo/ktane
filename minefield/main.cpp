#include <Arduino.h>

#include <LedControl.h>

#include "paths.h"

const uint8_t PIN_NORTH = 7;
const uint8_t PIN_EAST = 6;
const uint8_t PIN_SOUTH = 5;
const uint8_t PIN_WEST = 4;

const uint8_t PIN_DISARM_LED = 8;

LedControl display = LedControl(12, 11, 10, 1);

uint8_t progress = 0;

void setup()
{
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
}

void displayCell() {
  display.setLed(0, path[progress] / 8, path[progress] % 8, true);
}

void displayMaze() {
  display.clearDisplay(0);
  for (uint8_t i = 0; i <= progress && path[i] != 255; ++i)
    display.setLed(0, path[i] / 8, path[i] % 8, true);
}

uint8_t lastProgress = 255;
void loop()
{
  uint8_t cell = 255;
  if (digitalRead(PIN_NORTH))
  {
    while (digitalRead(PIN_NORTH));
    cell = path[progress] + 8;
  }
  else if (digitalRead(PIN_EAST))
  {
    while (digitalRead(PIN_EAST));
    cell = path[progress] + 1;
  }
  else if (digitalRead(PIN_SOUTH))
  {
    while (digitalRead(PIN_SOUTH));
    cell = path[progress] - 8;
  }
  else if (digitalRead(PIN_WEST))
  {
    while (digitalRead(PIN_WEST));
    cell = path[progress] - 1;
  }
  if (cell == path[progress+1])
    ++progress;
  else if (cell != 255)
  {
    digitalWrite(PIN_DISARM_LED, 1);
    delay(500);
    digitalWrite(PIN_DISARM_LED, 0);
    delay(500);
  }

  if (progress != lastProgress)
  {
    displayCell();
    lastProgress = progress;
  }
}

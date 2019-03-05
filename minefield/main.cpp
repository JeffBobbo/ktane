#include <Arduino.h>

#include <LedControl.h>

const uint8_t PIN_NORTH = 7;
const uint8_t PIN_EAST = 6;
const uint8_t PIN_SOUTH = 5;
const uint8_t PIN_WEST = 4;

const uint8_t PIN_DISARM_LED = 8;

LedControl display = LedControl(12, 11, 10, 1);

uint8_t path[36] = {
  0 * 8 + 4,
  1 * 8 + 4, 1 * 8 + 5,
  2 * 8 + 5, 2 * 8 + 6,
  3 * 8 + 6,
  4 * 8 + 6, 4 * 8 + 7,
  5 * 8 + 7,
  6 * 8 + 7, 6 * 8 + 6, 6 * 8 + 5, 6 * 8 + 4,
  5 * 8 + 4,
  4 * 8 + 4, 4 * 8 + 3,
  3 * 8 + 3,
  2 * 8 + 3, 2 * 8 + 2, 2 * 8 + 1,
  3 * 8 + 1, 3 * 8 + 0,
  4 * 8 + 0,
  5 * 8 + 0, 5 * 8 + 1,
  6 * 8 + 1, 6 * 8 + 2,
  7 * 8 + 2,
  255
};
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
    displayMaze();
    lastProgress = progress;
  }
}

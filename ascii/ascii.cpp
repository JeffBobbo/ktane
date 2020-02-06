#include <Arduino.h>

const uint8_t PIN_DISARM_LED = 4;
const uint8_t PIN_ENTER = 5;
const uint8_t PIN_SWITCHES[] = {6, 7, 8, 9, 10, 11, 12, 13};
const size_t NUM_SWITCHES = sizeof(PIN_SWITCHES);

void setup()
{
  pinMode(PIN_DISARM_LED, OUTPUT);

  pinMode(PIN_ENTER, INPUT);
  for (size_t i = 0; i < sizeof(PIN_SWITCHES); ++i)
    pinMode(PIN_SWITCHES[i], INPUT);

  Serial.begin(9600);
}

void loop()
{
  if (digitalRead(PIN_ENTER))
  {
    uint8_t value = 0x0;
    for (size_t i = 0; i < sizeof(PIN_SWITCHES); ++i)
      value = (value >> 1) | (digitalRead(PIN_SWITCHES[i]) << (NUM_SWITCHES - 1));
    Serial.println(value, HEX);
    while (digitalRead(PIN_ENTER));
  }
}

#include <Arduino.h>

#include <LiquidCrystal.h>

#include "shared/debounce.h"

const uint8_t PIN_DISARM_LED = 4;
const uint8_t PIN_ENTER = 5;
const uint8_t PIN_SWITCHES[] = {6, 7, 8, 9, 10, 11, 12, 13};
const size_t NUM_SWITCHES = sizeof(PIN_SWITCHES);

// LCD pins
const uint8_t PIN_RS = 2;
const uint8_t PIN_ENABLE = 3;
const uint8_t PIN_D4 = A3;
const uint8_t PIN_D5 = A2;
const uint8_t PIN_D6 = A1;
const uint8_t PIN_D7 = A0;

Debounce enter(PIN_ENTER);

LiquidCrystal lcd(PIN_RS, PIN_ENABLE, PIN_D4, PIN_D5, PIN_D6, PIN_D7);


const char* code = "ABCDEF";
size_t progress = 0;
void setup()
{
  randomSeed(427);

  pinMode(PIN_DISARM_LED, OUTPUT);
  digitalWrite(PIN_DISARM_LED, 1);

  enter.init();
  for (size_t i = 0; i < sizeof(PIN_SWITCHES); ++i)
    pinMode(PIN_SWITCHES[i], INPUT);

  lcd.begin(16, 2);


  Serial.begin(9600);
}

void loop()
{
  if (progress < 8)
  {
    enter.update();

    lcd.setCursor(0, 0);
    lcd.print(code);

    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);

    uint8_t value = 0x0;
    for (size_t i = 0; i < sizeof(PIN_SWITCHES); ++i)
      value = (value >> 1) | (digitalRead(PIN_SWITCHES[i]) << (NUM_SWITCHES - 1));
    lcd.print(value, BIN);

    lcd.setCursor(14, 1);
    lcd.print(progress, DEC);

    if (enter.is_released())
    {
      const char c = static_cast<char>(value);
      if (c == code[progress])
      {
        ++progress;
        if (progress == 8)
          digitalWrite(PIN_DISARM_LED, 0);
      }
      else
      {
        digitalWrite(PIN_DISARM_LED, 0);
        delay(200);
        digitalWrite(PIN_DISARM_LED, 1);
      }
    }
  }
}

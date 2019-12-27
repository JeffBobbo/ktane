#include <Arduino.h>

const uint8_t PIN_SAFEZONE_0 = 4;
const uint8_t PIN_SAFEZONE_1 = 5;
const uint8_t PIN_TRACK = 2;

const uint8_t PIN_LED = 6;

const uint32_t CONTACT_TIMEOUT = 500;

void setup()
{
  pinMode(PIN_SAFEZONE_0, INPUT);
  pinMode(PIN_SAFEZONE_1, INPUT);
  pinMode(PIN_TRACK, INPUT);

  pinMode(PIN_LED, OUTPUT);
}


bool inProgress = false;
bool done = false;
uint32_t safe = 0;

enum class Position
{
  LEFT,
  RIGHT
};

Position start;

void loop()
{
  const uint32_t now = millis();

  if (done)
  {
    while (1)
    {
      digitalWrite(PIN_LED, 1);
      digitalWrite(LED_BUILTIN, 0);
      delay(250);
      digitalWrite(PIN_LED, 0);
      digitalWrite(LED_BUILTIN, 1);
      delay(250);
    }
  }

  if (inProgress == false)
  {
    if (digitalRead(PIN_SAFEZONE_0))
    {
      start = Position::LEFT;
      inProgress = true;
      safe = now;
      while (digitalRead(PIN_SAFEZONE_0));
    }
    else if (digitalRead(PIN_SAFEZONE_1))
    {
      start = Position::RIGHT;
      inProgress = true;
      safe = now;
      while (digitalRead(PIN_SAFEZONE_1));
    }
  }
  else
  {
    if (start == Position::LEFT && digitalRead(PIN_SAFEZONE_1) || digitalRead(PIN_SAFEZONE_0))
      done = true;
    else if (digitalRead(PIN_TRACK) && safe < now + CONTACT_TIMEOUT)
    {
      safe = now + CONTACT_TIMEOUT;
      digitalWrite(PIN_LED, 1);
    }
    else if (safe >= now + CONTACT_TIMEOUT)
    {
      digitalWrite(PIN_LED, 0);
    }
  }
  digitalWrite(LED_BUILTIN, inProgress);
}

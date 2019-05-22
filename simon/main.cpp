#include "shared/module.h"
#include "shared/util.h"

const Address addr = address::SIMON;
const uint8_t PIN_DISARM_LED = 10;

// outputs
const uint8_t LED_R = 5;
const uint8_t LED_G = 4;
const uint8_t LED_Y = 3;
const uint8_t LED_B = 2;
const uint8_t LEDS[] = {LED_R, LED_G, LED_Y, LED_B};

const uint8_t PIN_BUZZER = 12;
const uint16_t TONES[] = {587, 659, 698, 784};

// inputs
const uint8_t PIN_R = 9;
const uint8_t PIN_G = 8;
const uint8_t PIN_Y = 7;
const uint8_t PIN_B = 6;
const uint8_t PINS[] = {PIN_R, PIN_G, PIN_Y, PIN_B};

const uint8_t PIN_SEED = 0;

bool showing;
const uint8_t MAX_CODE_LENGTH = 4;
uint8_t code[MAX_CODE_LENGTH] = {0};
uint8_t current; // current button we're pressing for
uint8_t progress; // our total progress (i.e., how many to flash)

const uint32_t FLASH_TIME = 500;
const uint32_t PAUSE_TIME = 500;
const uint32_t REPEAT_TIME = 5000;

uint32_t start;

void generateCode()
{
  for (uint8_t i = 0; i < MAX_CODE_LENGTH; ++i)
    code[i] = random(4);
}

uint8_t mapCode(const uint8_t value)
{
  if (util::countNumbers(indicators.serial) > 4)
    return (value + 1 + indicators.strikes) % 4;
  return (value + 3 + indicators.strikes) % 4;
}

void initialise()
{
  // setup pins
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  pinMode(PIN_R, INPUT);
  pinMode(PIN_G, INPUT);
  pinMode(PIN_Y, INPUT);
  pinMode(PIN_B, INPUT);

  // seed
  randomSeed(analogRead(PIN_SEED));
}

void reset()
{
  digitalWrite(LED_R, 0);
  digitalWrite(LED_G, 0);
  digitalWrite(LED_Y, 0);
  digitalWrite(LED_B, 0);

  current = 0;
  progress = 0;

  generateCode();
  showing = true;
}

void onIndicators()
{
  if (state == ModuleState::INITIALISATION)
    state = ModuleState::READY;
}

void arm()
{
  start = millis() + 2500;
}

void stop()
{
  // turn the LEDs off
  for (uint8_t i = 0; i < 4; ++i)
    digitalWrite(LEDS[i], 0);
  noTone(PIN_BUZZER);
}

void idle()
{
  if (showing)
  {
    const uint32_t period = (progress+1) * (FLASH_TIME + PAUSE_TIME) + REPEAT_TIME;

    const uint32_t now = (millis() - start) % period;
    if (now < period - REPEAT_TIME)
    {
      const uint32_t led = now / (FLASH_TIME + PAUSE_TIME);

      if (now % (FLASH_TIME + PAUSE_TIME) < FLASH_TIME)
      {
        digitalWrite(LEDS[code[led]], 1);
        tone(PIN_BUZZER, TONES[code[led]]);
      }
      else
      {
        stop();
      }
    }
  }

  for (uint8_t i = 0; i < 4; ++i)
  {
    if (digitalRead(PINS[i]))
    {
      while (digitalRead(PINS[i]));
      showing = false;
      if (code[current] != mapCode(i))
      {
        strike();
        start = millis();
        current = 0;
      }
      else
      {
        if (current == progress)
        {
          current = 0;
          ++progress;
          start = millis();
          showing = true;
        }
        else
        {
          ++current;
        }
      }
    }
  }
  if (progress == MAX_CODE_LENGTH)
  {
    disarm();
    for (uint8_t i = 0; i < 4; ++i)
      digitalWrite(LEDS[i], 0);
  }
}

void defuse()
{
}

void detonate()
{
}

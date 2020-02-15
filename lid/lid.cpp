#define UTILITY_MODULE
#include "shared/module.h"

#include <FastLED.h>

const Address addr = address::LID;

const uint8_t PIN_RELAY = 4;
const uint8_t PIN_LEDS = 5;

const size_t NUM_LEDS = 1;
CRGB leds[NUM_LEDS];

// if this is not deemed white enough, 0xDFFFDF may be better
const uint32_t LED_CORRECTION = 0xFFFFFF;

uint32_t start;
void initialise()
{
  pinMode(PIN_RELAY, OUTPUT);

  FastLED.addLeds<WS2812, PIN_LEDS, GRB>(leds, NUM_LEDS).setCorrection(LED_CORRECTION);
  FastLED.setBrightness(255);

  reset();
}

void reset()
{
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void onIndicators()
{
}

void arm()
{
  start = millis();
  fill_solid(leds, NUM_LEDS, CRGB::White);
  FastLED.show();
}

void idle()
{
  const uint32_t remaining = ((3L * 60L * 1000L) + start) - millis();
  digitalWrite(PIN_RELAY, ((remaining) / (remaining > 60L * 1000L ? 1000 : 500)) % 2);
}

void defuse()
{
  fill_solid(leds, NUM_LEDS, 0x1F3F7F);
  FastLED.show();
}

void detonate()
{
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
}

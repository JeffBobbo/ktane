#define UTILITY_MODULE
#include "shared/module.h"

const Address addr = address::TRAFFIC;

const uint8_t PIN_RED = 6;
const uint8_t PIN_AMBER = 5;
const uint8_t PIN_GREEN = 4;

const uint32_t TIME_STOP = 5000;
const uint32_t TIME_READY = 5000;
const uint32_t TIME_GO = 30000;
const uint32_t TIME_STOPPING = 5000;
const uint32_t TIME_PERIOD = TIME_STOP + TIME_READY + TIME_GO + TIME_STOPPING;

uint8_t lastDisarmed;

void initialise()
{
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_AMBER, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
}

void reset()
{
  digitalWrite(PIN_RED, 0);
  digitalWrite(PIN_AMBER, 0);
  digitalWrite(PIN_GREEN, 0);

  lastDisarmed = 0;
  forceGo = false;
}

void onIndicators()
{
  if (state == ModuleState::INITIALISATION)
    state = ModuleState::READY;
}

void arm()
{
}

enum class TrafficState
{
  STOP,
  READY,
  GO,
  STOPPING
};

bool forceGo = false;
TrafficState trafficState()
{
  const uint32_t now = millis() % TIME_PERIOD;

  if (!forceGo && now < TIME_STOP)
    return TrafficState::STOP;
  if (!forceGo && now < TIME_STOP + TIME_READY)
    return TrafficState::READY;
  if (forceGo || now < TIME_STOP + TIME_READY + TIME_GO)
  {
    if (indicators.disarmed == indicators.modules - 1)
      forceGo = true;
    return TrafficState::GO;
  }
  return TrafficState::STOPPING;
}

void idle()
{
  const TrafficState s = trafficState();

  digitalWrite(PIN_RED, s == TrafficState::STOP || s == TrafficState::READY);
  digitalWrite(PIN_AMBER, s == TrafficState::READY || s == TrafficState::STOPPING);
  digitalWrite(PIN_GREEN, s == TrafficState::GO);

  if (indicators.disarmed != lastDisarmed && (s == TrafficState::STOP || s == TrafficState::READY))
    strike();
  lastDisarmed = indicators.disarmed;
}

void detonate()
{
}

void defuse()
{
}

#ifndef MODULE_H_INCLUDE
#define MODULE_H_INCLUDE

#include <Arduino.h>
#include <Wire.h>

#include "address.h"
#include "message.h"
#include "opcode.h"

void disarm();
void strike();

void initialise();
void reset();
void onIndicators();
void arm();
void idle();
void defuse();
void detonate();

extern const Address addr;
#ifndef UTILITY_MODULE
extern const uint8_t PIN_DISARM_LED;
#endif
Indicators indicators;

ModuleState state;
uint8_t strikes;

void disarm()
{
  state = ModuleState::DISARMED;
  #ifndef UTILITY_MODULE
  digitalWrite(PIN_DISARM_LED, 0);
  #endif
}

void strike()
{
  ++strikes;
}

void receiveEvent(int count)
{
  if (count == 0) // if count is 0, this is an ACK from master, so just return
    return;

  Message msg;

  for (int i = 0; i < count; ++i)
    reinterpret_cast<uint8_t*>(&msg)[i] = Wire.read();

  switch (msg.opcode)
  {
    /*
    case OpCode::CONFIGURE:
      memcpy(&configuration, msg.data, sizeof(configuration));
      configure();
    break;
    */
    case OpCode::ARM:
      state = ModuleState::ARMED;
      #ifndef UTILITY_MODULE
      digitalWrite(PIN_DISARM_LED, 1);
      #endif
      arm();
    break;
    case OpCode::DEFUSED:
      state = ModuleState::STOP;
      defuse();
    break;
    case OpCode::EXPLODED:
      state = ModuleState::STOP;
      detonate();
    break;
    case OpCode::RESET:
      state = ModuleState::INITIALISATION;
      strikes = 0;
      reset();
    break;
    case OpCode::INDICATORS:
      memcpy(&indicators, msg.data, sizeof(indicators));
      onIndicators();
    break;
  }
}

void requestEvent()
{
  Status status = {state, strikes};
  Wire.write(reinterpret_cast<uint8_t*>(&status), sizeof(status));

  // reset this to zero once it's been sent
  strikes = 0;
}

void setup()
{
  // setup comms
  Wire.begin(addr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  // setup disarm LED
  #ifndef UTILITY_MODULE
  pinMode(PIN_DISARM_LED, OUTPUT);
  #endif

  // pass off to custom initialisation
  initialise();
}

void loop()
{
  // noop while not ARMED
  if (state != ModuleState::ARMED)
    return;

  idle();
}


#endif

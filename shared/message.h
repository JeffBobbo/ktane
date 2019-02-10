#ifndef MESSAGE_H_INCLUDE
#define MESSAGE_H_INCLUDE

#include <stdint.h>

#include "opcode.h"

struct Message
{
  OpCode opcode;
  uint8_t data[31];
};

enum class BaseState : uint8_t
{
  INITIALISATION, // do/doing setup
  READY,
  ARMED, // gameplay is running
  DEFUSED, // win state
  EXPLODED // lose state
};

enum class ModuleState : uint8_t
{
  INITIALISATION,
  READY,
  ARMED,
  DISARMED
};

struct Status
{
  ModuleState state;
  uint8_t strikes;
};

#endif

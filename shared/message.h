#ifndef MESSAGE_H_INCLUDE
#define MESSAGE_H_INCLUDE

#include <stdint.h>

#include "config.h"
#include "opcode.h"
#include "typedefs.h"

struct Message
{
  Message() : opcode(OpCode::NO_OP), data{0} {}
  Message(OpCode op) : opcode(op), data{0} {}

  OpCode opcode;
  uint8_t data[31];
};

enum class BaseState : uint8_t
{
  INITIALISATION, // do/doing setup
  READY,
  ARMED, // gameplay is running
  DEFUSED, // win state
  EXPLODED, // lose state
  CONFIG // config mode, not in normal flow
};

enum class ModuleState : uint8_t
{
  INITIALISATION, // while the module is starting up
  READY, // once it's started up, but waiting for base
  ARMED, // base has issued ARM message, game in progress
  DISARMED, // this module has been successfully disarmed
  STOP, // game over state, either defused or exploded
  CONFIG // config mode, not in normal flow
};

struct Status
{
  ModuleState state;
  uint8_t strikes;
};

struct Indicators
{
  uint8_t numerical;
  uint8_t binary;
  uint8_t strikes;
  uint8_t modules;
  uint8_t disarmed;
  char serial[SERIAL_LENGTH+1];
};

struct Configuration
{
  bool relay;
  uint8_t max_strikes;
  millis_t time_allowed;
};

#endif

#ifndef OPCODE_H_INCLUDE
#define OPCODE_H_INCLUDE

#include <stdint.h>

enum class OpCode : uint8_t
{
  NO_OP, // just in case?
  ARM, // instructs a module to arm itself
  DEFUSED, // instructs a module to stop because it's been defused
  EXPLODED // instructs a module to stop because it's exploded
};

#endif

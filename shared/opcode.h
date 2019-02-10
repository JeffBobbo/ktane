#ifndef OPCODE_H_INCLUDE
#define OPCODE_H_INCLUDE

#include <stdint.h>

enum class OpCode : uint8_t
{
  NO_OP,
  ARM, // instructs a module to arm itself
  DEFUSED, // instructs a module to stop because it's been defused
  EXPLODED, // instructs a module to stop because it's exploded
  VERSION // xfer of version string
};

String toString(const OpCode op)
{
  switch (op)
  {
    case OpCode::ARM:
      return "ARM";
    case OpCode::DEFUSED:
      return "DEFUSED";
    case OpCode::EXPLODED:
      return "EXPLODED";
    case OpCode::VERSION:
      return "VERSION";
  }
  return "???";
}

#endif

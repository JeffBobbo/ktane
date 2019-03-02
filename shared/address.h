#ifndef ADDRESS_H_INCLUDE
#define ADDRESS_H_INCLUDE

#include <stdint.h>

using Address = uint8_t;

namespace address
{
  const Address BROADCAST = 0;
  const Address BASE = 8;
  const Address SAFE = 9;
  const Address SIMON = 10;
  const Address DEADMAN = 11;

  // extra peripherals
  const Address OLED_DISPLAY = 0x3C; // 60

  // a list of modules we should broadcast to
  const Address modules[] = {
    SAFE,
    SIMON,
    DEADMAN
  };
  const size_t NUM_MODULES = sizeof(modules) / sizeof(Address);
};

#endif

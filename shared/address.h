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
  const Address MINEFIELD = 12;

  // extra peripherals
  const Address OLED_DISPLAY = 0x3C; // 60
  const Address INDICATORS = 61;

  // a list of modules we should broadcast to
  const Address modules[] = {
    SAFE,
    SIMON,
    DEADMAN,
    MINEFIELD
  };
  const size_t NUM_MODULES = sizeof(modules) / sizeof(Address);

  const Address needy[] = {
    DEADMAN
  };

  bool isNeedy(const Address module)
  {
    for (size_t i = 0; i < sizeof(needy) / sizeof(Address); ++i)
    {
      if (module == needy[i])
        return true;
    }
    return false;
  }
};

#endif

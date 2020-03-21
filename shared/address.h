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
  const Address AUTHORIZATION = 13;
  const Address TRAFFIC = 14;
  const Address FIZZBUZZ = 15;
  const Address MARITIME = 16;
  const Address ASCII = 17;

  // extra peripherals
  const Address OLED_DISPLAY = 0x3C; // 60
  const Address INDICATORS = 61;
  const Address LID = 62;
  const Address CONFIG = 63;
  const Address RTC = 0x68;

  // a list of modules we should broadcast to
  const Address modules[] = {
    SAFE,
    SIMON,
    DEADMAN,
    MINEFIELD,
    AUTHORIZATION,
    TRAFFIC,
    FIZZBUZZ,
    MARITIME,
    ASCII
  };
  const uint8_t NUM_MODULES = sizeof(modules) / sizeof(Address);

  const Address needy[] = {
    DEADMAN,
    TRAFFIC,
    FIZZBUZZ
  };

  bool isNeedy(const Address module)
  {
    for (uint8_t i = 0; i < sizeof(needy) / sizeof(Address); ++i)
    {
      if (module == needy[i])
        return true;
    }
    return false;
  }
};

#endif

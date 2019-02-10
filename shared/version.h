#ifndef VERSION_H_INCLUDE
#define VERSION_H_INCLUDE

#include <stdint.h>

const size_t VERSION_LENGTH = 8;

namespace version
{
  char version[VERSION_LENGTH+1] = {0};
  void generate()
  {
    for (uint8_t i = 0; i < VERSION_LENGTH; ++i)
    {
      const uint8_t c = random(46);
      if (c < 26)
        version[i] = 'a' + c;
      else if (c < 46)
        version[i] = '0' + (c - 26) % 10;
      else
        version[i] = '?';
    }
    version[VERSION_LENGTH] = 0;
  }

  uint8_t countVowel()
  {
    const size_t len = strlen(version);
    uint8_t count = 0;
    for (size_t i = 0; i < len; ++i)
    {
      const char c = version[i];
      if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
        ++count;
    }
    return count;
  }
  bool hasVowel()
  {
    return countVowel();
  }

  uint8_t countConsonant()
  {
    const size_t len = strlen(version);
    uint8_t count = 0;
    for (size_t i = 0; i < len; ++i)
    {
      const char c = version[i];
      if ((c > 'a' && c < 'e') ||
          (c > 'e' && c < 'i') ||
          (c > 'i' && c < 'o') ||
          (c > 'o' && c < 'u') ||
          (c > 'u' && c <= 'z'))
        ++count;
    }
    return count;  
  }

  bool hasConsonant()
  {
    return countConsonant();
  }

  uint8_t countEven()
  {
    const size_t len = strlen(version);
    uint8_t count = 0;
    for (size_t i = 0; i < len; ++i)
    {
      const char c = version[i];
      if (isdigit(c) && (c - '0') % 2 == 0)
        ++count;
    }
    return count;  
  }

  bool hasEven()
  {
    return countEven();
  }

  uint8_t countOdd()
  {
    const size_t len = strlen(version);
    uint8_t count = 0;
    for (size_t i = 0; i < len; ++i)
    {
      const char c = version[i];
      if (isdigit(c) && (c - '0') % 2 == 1)
        ++count;
    }
    return count;
  }
  bool hasOdd()
  {
    return countOdd();
  }

  int sum()
  {
    const size_t len = strlen(version);
    int sum = 0;
    for (size_t i = 0; i < len; ++i)
    {
      const char c = version[i];
      if (isdigit(c))
        sum += c - '0';
    }

    return sum;
  }
}

#endif

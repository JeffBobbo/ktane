#ifndef UTIL_H_INCLUDE
#define UTIL_H_INCLUDE

#include <stdint.h>
namespace util
{
  uint8_t countVowels(const char* const str)
  {
    const size_t len = strlen(str);
    uint8_t count = 0;
    for (size_t i = 0; i < len; ++i)
    {
      const char c = str[i];
      if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
        ++count;
    }
    return count;
  }
  bool hasVowels(const char* const str)
  {
    return countVowels(str);
  }

  uint8_t countConsonants(const char* const str)
  {
    const size_t len = strlen(str);
    uint8_t count = 0;
    for (size_t i = 0; i < len; ++i)
    {
      const char c = str[i];
      if ((c > 'a' && c < 'e') ||
          (c > 'e' && c < 'i') ||
          (c > 'i' && c < 'o') ||
          (c > 'o' && c < 'u') ||
          (c > 'u' && c <= 'z'))
        ++count;
    }
    return count;  
  }

  bool hasConsonants(const char* const str)
  {
    return countConsonants(str);
  }

  uint8_t countEvens(const char* const str)
  {
    const size_t len = strlen(str);
    uint8_t count = 0;
    for (size_t i = 0; i < len; ++i)
    {
      const char c = str[i];
      if (isdigit(c) && (c - '0') % 2 == 0)
        ++count;
    }
    return count;  
  }

  bool hasEvens(const char* const str)
  {
    return countEvens(str);
  }

  uint8_t countOdds(const char* const str)
  {
    const size_t len = strlen(str);
    uint8_t count = 0;
    for (size_t i = 0; i < len; ++i)
    {
      const char c = str[i];
      if (isdigit(c) && (c - '0') % 2 == 1)
        ++count;
    }
    return count;
  }
  bool hasOdds(const char* const str)
  {
    return countOdds(str);
  }

  uint8_t countNumbers(const char* const str)
  {
    return countEvens(str) + countOdds(str);
  }

  int sum(const char* const str)
  {
    const size_t len = strlen(str);
    int sum = 0;
    for (size_t i = 0; i < len; ++i)
    {
      const char c = str[i];
      if (isdigit(c))
        sum += c - '0';
    }

    return sum;
  }
}

#endif

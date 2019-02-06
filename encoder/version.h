const char* const version = "8fecad7";

uint8_t version_countVowel()
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
bool version_hasVowel()
{
  return version_countVowel();
}

uint8_t version_countConsonant()
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

bool version_hasConsonant()
{
  return version_countConsonant();
}

uint8_t version_countEven()
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

bool version_hasEven()
{
  return version_countEven();
}

uint8_t version_countOdd()
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
bool version_hasOdd()
{
  return version_countOdd();
}

int version_sum()
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

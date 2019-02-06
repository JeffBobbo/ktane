const char* const version = "8fecad7";

bool version_hasVowel()
{
  const size_t len = strlen(version);
  for (size_t i = 0; i < len; ++i)
  {
    const char c = version[i];
    if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
      return true;
  }
  return false;
}

bool version_hasConsonant()
{
  const size_t len = strlen(version);
  for (size_t i = 0; i < len; ++i)
  {
    const char c = version[i];
    if ((c > 'a' && c < 'e') ||
        (c > 'e' && c < 'i') ||
        (c > 'i' && c < 'o') ||
        (c > 'o' && c < 'u') ||
        (c > 'u' && c <= 'z'))
      return true;
  }
  return false;
}

bool version_hasEven()
{
  const size_t len = strlen(version);
  for (size_t i = 0; i < len; ++i)
  {
    const char c = version[i];
    if (isdigit(c) && (c - '0') % 2 == 0)
      return true;
  }
  return false;
}

bool version_hasOdd()
{
  const size_t len = strlen(version);
  for (size_t i = 0; i < len; ++i)
  {
    const char c = version[i];
    if (isdigit(c) && (c - '0') % 2 == 1)
      return true;
  }
  return false;
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

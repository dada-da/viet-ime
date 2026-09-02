#include "utf8.h"

bool utf8_is_continuation(unsigned char b)
{
  return (b & 0xC0) == 0x80;
}

size_t utf8_char_len(unsigned char lead)
{
  if ((lead & 0x80) == 0x00)
    return 1; // 0xxxxxxx
  if ((lead & 0xE0) == 0xC0)
    return 2; // 110xxxxx
  if ((lead & 0xF0) == 0xE0)
    return 3; // 1110xxxx
  if ((lead & 0xF8) == 0xF0)
    return 4; // 11110xxx
  return 0;   // byte tiếp nối hoặc rác
}

size_t utf8_char_count(const std::string &s)
{
  size_t n = 0;

  for (unsigned char b : s)
  {
    if (!utf8_is_continuation(b))
    {
      ++n;
    }
  }

  return n;
}

size_t utf8_prev_boundary(const std::string &s, size_t pos)
{
  if (pos == 0)
  {
    return 0;
  }

  size_t i = pos;
  do
  {
    --i;
  } while (i > 0 && utf8_is_continuation(static_cast<unsigned char>(s[i])));

  return i;
}

bool utf8_is_valid(const std::string &s)
{
  size_t i = 0;

  while (i < s.size())
  {
    size_t len = utf8_char_len(static_cast<unsigned char>(s[i]));

    if (len == 0 || i + len > s.size())
    {
      return false;
    }

    for (size_t k = 1; k < len; ++k)
    {
      if (!utf8_is_continuation(static_cast<unsigned char>(s[i + k])))
      {
        return false;
      }
    }

    i += len;
  }

  return true;
}
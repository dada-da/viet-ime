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

std::u32string utf8_to_utf32(const std::string &s)
{
  std::u32string out;
  size_t i = 0;

  while (i < s.size())
  {
    unsigned char lead = static_cast<unsigned char>(s[i]);
    size_t len = utf8_char_len(lead);

    if (len == 0 || i + len > s.size())
    {
      out.push_back(0xFFFD);
      ++i;
      continue;
    }

    char32_t cp;
    switch (len)
    {
    case 1:
      cp = lead; // 0xxxxxxx → 7 bit
      break;
    case 2:
      cp = lead & 0x1F; // 110xxxxx → 5 bit
      break;
    case 3:
      cp = lead & 0x0F; // 1110xxxx → 4 bit
      break;
    default:
      cp = lead & 0x07; // 11110xxx → 3 bit
      break;
    }

    for (size_t k = 1; k < len; k++)
    {
      cp = (cp << 6) | (static_cast<unsigned char>(s[i + k]) & 0x3F);
    }

    out.push_back(cp);
    i += len;
  }

  return out;
}

std::string utf32_to_utf8(const std::u32string &s)
{
  std::string out;

  for (char32_t cp : s)
  {
    if (cp < 0x80)
    {
      out += static_cast<char>(cp);
    }
    else if (cp < 0x800)
    {
      out += static_cast<char>(0xC0 | (cp >> 6));
      out += static_cast<char>(0x80 | (cp & 0x3F));
    }
    else if (cp < 0x10000)
    {
      out += static_cast<char>(0xE0 | (cp >> 12));
      out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
      out += static_cast<char>(0x80 | (cp & 0x3F));
    }
    else
    {
      out += static_cast<char>(0xF0 | (cp >> 18));
      out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
      out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
      out += static_cast<char>(0x80 | (cp & 0x3F));
    }
  }

  return out;
}
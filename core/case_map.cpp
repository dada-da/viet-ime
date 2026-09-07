#include "case_map.h"

namespace vietime
{
  bool is_upper_ascii(char c)
  {
    return c >= 'A' && c <= 'Z';
  }

  char to_lower_ascii(char c)
  {
    return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
  }

  char32_t to_upper_viet(char32_t c)
  {
    switch (c)
    {
    case U'ă':
      return U'Ă'; // U+0103 -> U+0102
    case U'đ':
      return U'Đ'; // U+0111 -> U+0110
    case U'ĩ':
      return U'Ĩ'; // U+0129 -> U+0128
    case U'ũ':
      return U'Ũ'; // U+0169 -> U+0168
    case U'ơ':
      return U'Ơ'; // U+01A1 -> U+01A0
    case U'ư':
      return U'Ư'; // U+01B0 -> U+01AF
    default:
      break;
    }

    if (c >= U'a' && c <= U'z')
    {
      return c - 0x20;
    }

    if (c >= 0x00E0 && c <= 0x00FE && c != 0x00F7)
    {
      return c - 0x20;
    }

    if (c >= 0x1EA0 && c <= 0x1EF9 && (c % 2) == 1)
      return c - 1;

    return c;
  }
}
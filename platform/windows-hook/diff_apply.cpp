// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "diff_apply.h"

namespace vietime_hook
{
  namespace
  {
    bool is_cont(unsigned char b) { return (b & 0xC0) == 0x80; }

    std::size_t byte_at_cp(std::string_view s, std::size_t n)
    {
      std::size_t i = 0, cp = 0;
      while (i < s.size() && cp < n)
      {
        i++;
        while (i < s.size() && is_cont(static_cast<unsigned char>(s[i])))
          i++;
        cp++;
      }
      return i;
    }

    std::size_t cp_count(std::string_view s)
    {
      std::size_t n = 0;
      for (unsigned char b : s)
        if (!is_cont(b))
          n++;
      return n;
    }
  }

  std::size_t common_prefix_chars(std::string_view a, std::string_view b)
  {
    std::size_t i = 0, cp = 0;
    while (i < a.size() && i < b.size())
    {
      std::size_t start = i;
      std::size_t j = i + 1;
      while (j < a.size() && is_cont(static_cast<unsigned char>(a[j])))
        j++;

      std::size_t len = j - start;
      if (start + len > b.size())
        break;

      bool same = true;
      for (std::size_t k = 0; k < len; k++)
      {
        if (a[start + k] != b[start + k])
        {
          same = false;
          break;
        }
      }
      if (!same)
        break;

      i = j;
      cp++;
    }
    return cp;
  }

  Diff shortest_diff(std::string_view old_shown, std::string_view new_full)
  {
    std::size_t common = common_prefix_chars(old_shown, new_full);

    Diff d;
    d.backspaces = cp_count(old_shown) - common;

    std::size_t tail_start = byte_at_cp(new_full, common);
    d.tail = std::string(new_full.substr(tail_start));
    return d;
  }
}
// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "syllable.h"

namespace vietime
{

  bool is_vowel(char32_t c)
  {
    switch (c)
    {
    case U'a':
    case U'ă':
    case U'â':
    case U'e':
    case U'ê':
    case U'i':
    case U'o':
    case U'ô':
    case U'ơ':
    case U'u':
    case U'ư':
    case U'y':
      return true;
    default:
      return false;
    }
  }

  SyllableParts split_syllable(const std::u32string &s)
  {
    std::size_t i = 0;
    while (i < s.size() && !is_vowel(s[i]))
      ++i;

    std::size_t v = i;
    while (v < s.size() && is_vowel(s[v]))
      ++v;

    if (i > 0)
    {
      char32_t initial_last = s[i - 1];
      if ((initial_last == U'q' && s[i] == U'u') || ((initial_last == U'g' && s[i] == U'i') && v - i >= 2)) // voi qu se bo qua do q + u khong hop ly -> nucleus empty con voi gi thi i van la nguyen am hop le -> nucleus = i
        ++i;
    }

    SyllableParts p;
    p.initial = s.substr(0, i);
    p.nucleus = s.substr(i, v - i);
    p.coda = s.substr(v);
    p.nucleus_start = i;

    return p;
  }

  std::size_t find_tone_position(const std::u32string &s, TonePlacement tone_placement)
  {
    const SyllableParts p = split_syllable(s);
    const std::u32string &v = p.nucleus;

    if (v.empty())
      return NO_TONE_POS;

    // luật 1: ơ (ươ, uơ)
    if (auto k = v.find(U'ơ'); k != std::u32string::npos)
      return p.nucleus_start + k;

    // luật 2: ê (iê, yê, uyê)
    if (auto k = v.find(U'ê'); k != std::u32string::npos)
      return p.nucleus_start + k;

    // luật 3: nguyên âm mang dấu phụ khác
    // Phải sau luật 1: ươ có cả ư lẫn ơ, dấu vào ơ.
    if (auto k = v.find_first_of(U"ăâôư"); k != std::u32string::npos)
      return p.nucleus_start + k;

    // luật 4: âm chính chỉ 1 nguyên âm (a, i, e, ê, ư,...)
    if (v.size() == 1)
    {
      return p.nucleus_start;
    }

    // luật 4b: âm đệm + âm chính trần, kiểu mới (oà uý thay vì òa úy)
    if (tone_placement == PLACEMENT_MODERN)
    {
      if ((p.nucleus == U"oa" || p.nucleus == U"oe" || p.nucleus == U"uy") && p.coda.empty())
      {
        return p.nucleus_start + 1;
      }
    }

    // luật 5: có âm cuối toan → toán, hoan → hoàn
    if (!p.coda.empty())
    {
      return p.nucleus_start + v.size() - 1;
    }

    // luật 6: 3 nguyên âm, không âm cuối ngoai → ngoài
    if (v.size() == 3)
    {
      return p.nucleus_start + 1;
    }

    // luật 7: còn lại → nguyên âm đầu tiên cua → của, hoa → hòa, chia → chìa
    return p.nucleus_start;
  }

  bool has_vowel(const std::u32string &s)
  {
    const SyllableParts p = split_syllable(s);
    const std::u32string &v = p.nucleus;

    if (v.empty())
      return false;

    for (char32_t c : v)
    {
      if (is_vowel(c))
      {
        return true;
      }
    }

    return false;
  }
}
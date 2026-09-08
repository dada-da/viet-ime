// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "transform_engine.h"
#include "syllable.h"

namespace vietime
{
  namespace
  {
    char32_t horn_result(char32_t c)
    {
      switch (c)
      {
      case U'o':
        return U'ơ';
      case U'u':
        return U'ư';
      default:
        return 0;
      }
    }

    char32_t breve_result(char32_t c)
    {
      if (c == U'a')
      {
        return U'ă';
      }

      return 0;
    }

    char32_t horn_or_breve_result(char32_t c)
    {
      if (char32_t r = breve_result(c); r != 0)
      {
        return r;
      }

      return horn_result(c);
    }

    char32_t circumflex_result(char32_t c)
    {
      switch (c)
      {
      case U'a':
        return U'â';
      case U'e':
        return U'ê';
      case U'o':
        return U'ô';
      default:
        return 0;
      }
    }

    struct ModRule
    {
      char32_t (*map)(char32_t);
      bool pair_uo;
      char32_t must_equal;
    };

    bool rule_for_key(char key, InputMethod method, ModRule &out)
    {
      if (method == METHOD_VNI)
      {
        switch (key)
        {
        case '6':
          out = {circumflex_result, false, 0};
          return true;
        case '7':
          out = {horn_result, true, 0};
          return true;
        case '8':
          out = {breve_result, false, 0};
          return true;
        case '9':
          out = {nullptr, false, 0};
          return true;
        default:
          return false;
        }
      }

      switch (key)
      {
      case 'w':
        out = {horn_or_breve_result, true, 0};
        return true;
      case 'd':
        out = {nullptr, false, 0};
        return true;
      case 'a':
      case 'e':
      case 'o':
        out = {circumflex_result, false, static_cast<char32_t>(key)};
        return true;
      default:
        return false;
      }
    }
  } // namespace

  bool apply_modifier(std::u32string &base, char key, InputMethod method)
  {
    if (base.empty())
      return false;

    ModRule rule;
    if (!rule_for_key(key, method, rule))
      return false;

    if (rule.map == nullptr)
    {
      if (base.back() != U'd')
      {
        return false;
      }
      base.back() = U'đ';
      return true;
    }

    const SyllableParts p = split_syllable(base);
    const std::u32string &v = p.nucleus;

    if (v.empty())
    {
      return false;
    }

    if (rule.pair_uo)
    {
      for (std::size_t i = 0; i + 1 < v.size(); i++)
      {
        if (v[i] == U'u' && v[i + 1] == U'o')
        {
          base[p.nucleus_start + i] = U'ư';
          base[p.nucleus_start + i + 1] = U'ơ';
          return true;
        }
      }
    }

    for (std::size_t i = v.size(); i > 0; i--)
    {
      const char32_t c = v[i - 1];

      if (rule.must_equal != 0 && c != rule.must_equal)
        continue;

      if (char32_t r = rule.map(c); r != 0)
      {
        base[p.nucleus_start + i - 1] = r;
        return true;
      }
    }

    return false;
  }
} // namespace vietime
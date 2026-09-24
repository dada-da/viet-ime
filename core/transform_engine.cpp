// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <map>
#include <optional>

#include "transform_engine.h"
#include "syllable.h"
#include "rime_table.h"

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

    const std::map<char32_t, char32_t> kTransformTable = {
        {U'ă', U'a'},
        {U'â', U'a'},
        {U'ê', U'e'},
        {U'ô', U'o'},
        {U'ơ', U'o'},
        {U'ư', U'u'},
    };

    bool is_transform(char32_t c)
    {
      auto candidate_key = kTransformTable.find(c);

      return candidate_key != kTransformTable.end();
    }

    bool is_candidate(const char32_t base_char, const ModRule &rule)
    {
      char32_t result = rule.map(base_char);

      if (result != 0 && (rule.must_equal == 0 || rule.must_equal == base_char))
      {
        return true;
      }

      return false;
    }
  } // namespace

  ModResult apply_modifier(std::u32string &base, char key, InputMethod method)
  {
    ModResult mod_result;

    if (base.empty())
      return mod_result;

    ModRule rule;
    if (!rule_for_key(key, method, rule))
      return mod_result;

    if (rule.map == nullptr)
    {
      if (base.front() != U'd')
        return mod_result;

      mod_result.applied = true;
      mod_result.old_chars[0] = base.front();
      mod_result.pos = 0;
      mod_result.count = 1;

      base.front() = U'đ';

      return mod_result;
    }

    const SyllableParts p = split_syllable(base);
    const std::u32string &v = p.nucleus;

    if (v.empty())
      return mod_result;

    if (rule.pair_uo)
    {
      for (std::size_t i = 0; i + 1 < v.size(); i++)
      {
        if (v[i] == U'u' && v[i + 1] == U'o')
        {
          base[p.nucleus_start + i] = U'ư';
          base[p.nucleus_start + i + 1] = U'ơ';

          mod_result.applied = true;
          mod_result.old_chars[0] = v[i];
          mod_result.old_chars[1] = v[i + 1];
          mod_result.count = 2;
          mod_result.pos = p.nucleus_start + i;

          return mod_result;
        }
      }
    }

    for (std::size_t i = v.size(); i > 0; i--)
    {
      const char32_t c = v[i - 1];

      if (is_transform(c))
      {
        return mod_result;
      }

      if (!is_candidate(c, rule))
        continue;

      const std::size_t nucleus_index = i - 1;

      if (char32_t r = rule.map(c); r != 0)
      {
        std::u32string probe(v);
        probe[nucleus_index] = r;

        if (!is_vowel_cluster_prefix(probe))
          continue;

        base[p.nucleus_start + nucleus_index] = r;
        mod_result.applied = true;
        mod_result.old_chars[0] = c;
        mod_result.pos = p.nucleus_start + nucleus_index;
        mod_result.count = 1;

        return mod_result;
      }
    }

    return mod_result;
  }

  bool is_tone_removal_key(char key, InputMethod method)
  {
    if (method == METHOD_VNI)
      return key == '0';

    return key == 'z';
  }

  bool is_modifier_block_key(char key, InputMethod method)
  {
    if (method == METHOD_TELEX)
    {
      return key == 'w' || key == 'd';
    }

    return key == '6' || key == '7' || key == '8' || key == '9';
  }

  bool is_word_boundary(char key)
  {
    if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z') || (key >= '0' && key <= '9'))
      return false;

    return true;
  }
} // namespace vietime
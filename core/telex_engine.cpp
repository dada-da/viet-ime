#include "telex_engine.h"
#include "syllable.h"

namespace vietime
{
  namespace
  {
    char32_t w_result(char32_t c)
    {
      switch (c)
      {
      case U'a':
        return U'ă';
      case U'o':
        return U'ơ';
      case U'u':
        return U'ư';
      default:
        return 0;
      }
    }

    char32_t double_result(char32_t c, char key)
    {
      if (static_cast<char32_t>(key) != c)
      {
        return 0;
      }

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
  } // namespace

  bool apply_modifier(std::u32string &base, char key)
  {
    if (base.empty())
      return false;

    const char32_t last = base.back();

    if (key == 'd' && last == U'd')
    {
      base.back() = U'đ';
      return true;
    }

    const SyllableParts p = split_syllable(base);
    const std::u32string &v = p.nucleus;

    if (v.empty())
    {
      return false;
    }

    if (key == 'w')
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

      for (std::size_t i = v.size(); i > 0; i--)
      {
        if (char32_t r = w_result(v[i - 1]); r != 0)
        {
          base[p.nucleus_start + i - 1] = r;
          return true;
        }
      }

      return false;
    }

    for (std::size_t i = v.size(); i > 0; i--)
    {
      if (char32_t r = double_result(v[i - 1], key); r != 0)
      {
        base[p.nucleus_start + i - 1] = r;
        return true;
      }
    }

    return false;
  }
} // namespace vietime
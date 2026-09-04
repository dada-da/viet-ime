#pragma once
#include <string>
#include <cstddef>

namespace vietime
{
  inline constexpr std::size_t NO_TONE_POS = static_cast<std::size_t>(-1);

  struct SyllableParts
  {
    std::u32string initial;
    std::u32string nucleus;
    std::u32string coda;
    std::size_t nucleus_start;
  };

  bool is_vowel(char32_t c);
  SyllableParts split_syllable(const std::u32string &s);
  std::size_t find_tone_position(const std::u32string &s);
} // namespace vietime
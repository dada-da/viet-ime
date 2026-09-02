#include "tone_table.h"

#include <map>
#include <vector>

namespace
{

  const std::map<char, std::vector<std::string>> kToneTable = {
      {'a', {"á", "à", "ả", "ã", "ạ"}},
      {'e', {"é", "è", "ẻ", "ẽ", "ẹ"}},
      {'i', {"í", "ì", "ỉ", "ĩ", "ị"}},
      {'o', {"ó", "ò", "ỏ", "õ", "ọ"}},
      {'u', {"ú", "ù", "ủ", "ũ", "ụ"}},
      {'y', {"ý", "ỳ", "ỷ", "ỹ", "ỵ"}},
  };
}

Tone tone_from_telex(char key)
{
  switch (key)
  {
  case 's':
    return TONE_SAC;
  case 'f':
    return TONE_HUYEN;
  case 'r':
    return TONE_HOI;
  case 'x':
    return TONE_NGA;
  case 'j':
    return TONE_NANG;

  default:
    return TONE_NONE;
  }
}

Tone tone_from_vni(char key)
{
  switch (key)
  {
  case '1':
    return TONE_SAC;
  case '2':
    return TONE_HUYEN;
  case '3':
    return TONE_HOI;
  case '4':
    return TONE_NGA;
  case '5':
    return TONE_NANG;

  default:
    return TONE_NONE;
  }
}

std::string apply_tone_to_vowel(char vowel, Tone tone)
{
  if (tone == TONE_NONE)
  {
    return std::string();
  }

  auto it = kToneTable.find(vowel);

  if (it == kToneTable.end())
  {
    return std::string();
  }

  return it->second[static_cast<size_t>(tone)];
}
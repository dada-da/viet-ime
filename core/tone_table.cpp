#include "tone_table.h"

#include <map>
#include <array>

namespace
{

  const std::map<char32_t, std::array<char32_t, 5>> kToneTable = {
      {U'a', {U'á', U'à', U'ả', U'ã', U'ạ'}},
      {U'ă', {U'ắ', U'ằ', U'ẳ', U'ẵ', U'ặ'}},
      {U'â', {U'ấ', U'ầ', U'ẩ', U'ẫ', U'ậ'}},
      {U'e', {U'é', U'è', U'ẻ', U'ẽ', U'ẹ'}},
      {U'ê', {U'ế', U'ề', U'ể', U'ễ', U'ệ'}},
      {U'i', {U'í', U'ì', U'ỉ', U'ĩ', U'ị'}},
      {U'o', {U'ó', U'ò', U'ỏ', U'õ', U'ọ'}},
      {U'ô', {U'ố', U'ồ', U'ổ', U'ỗ', U'ộ'}},
      {U'ơ', {U'ớ', U'ờ', U'ở', U'ỡ', U'ợ'}},
      {U'u', {U'ú', U'ù', U'ủ', U'ũ', U'ụ'}},
      {U'ư', {U'ứ', U'ừ', U'ử', U'ữ', U'ự'}},
      {U'y', {U'ý', U'ỳ', U'ỷ', U'ỹ', U'ỵ'}},
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

char32_t apply_tone_to_vowel(char32_t vowel, Tone tone)
{
  if (tone == TONE_NONE)
  {
    return char32_t();
  }

  auto it = kToneTable.find(vowel);

  if (it == kToneTable.end())
  {
    return char32_t();
  }

  return it->second[static_cast<size_t>(tone)];
}
#include <map>
#include "normalize.h"
#include "utf8.h"

namespace
{
  // Cột: không dấu, huyền, sắc, hỏi, ngã, nặng.
  const char32_t *const kToneRows[] = {
      U"aàáảãạ",
      U"ăằắẳẵặ",
      U"âầấẩẫậ",
      U"eèéẻẽẹ",
      U"êềếểễệ",
      U"iìíỉĩị",
      U"oòóỏõọ",
      U"ôồốổỗộ",
      U"ơờớởỡợ",
      U"uùúủũụ",
      U"ưừứửữự",
      U"yỳýỷỹỵ",
      U"AÀÁẢÃẠ",
      U"ĂẰẮẲẴẶ",
      U"ÂẦẤẨẪẬ",
      U"EÈÉẺẼẸ",
      U"ÊỀẾỂỄỆ",
      U"IÌÍỈĨỊ",
      U"OÒÓỎÕỌ",
      U"ÔỒỐỔỖỘ",
      U"ƠỜỚỞỠỢ",
      U"UÙÚỦŨỤ",
      U"ƯỪỨỬỮỰ",
      U"YỲÝỶỸỴ",
  };

  // (chữ gốc, dấu phụ) -> chữ có dấu phụ
  const std::map<std::pair<char32_t, char32_t>, char32_t> kDiacritic = {
      {{U'a', 0x0302}, U'â'},
      {{U'a', 0x0306}, U'ă'},
      {{U'e', 0x0302}, U'ê'},
      {{U'o', 0x0302}, U'ô'},
      {{U'o', 0x031B}, U'ơ'},
      {{U'u', 0x031B}, U'ư'},
      {{U'A', 0x0302}, U'Â'},
      {{U'A', 0x0306}, U'Ă'},
      {{U'E', 0x0302}, U'Ê'},
      {{U'O', 0x0302}, U'Ô'},
      {{U'O', 0x031B}, U'Ơ'},
      {{U'U', 0x031B}, U'Ư'},
  };

  // dấu thanh tổ hợp -> chỉ số cột trong kToneRows
  const std::map<char32_t, size_t> kToneCol = {
      {0x0300, 1},
      {0x0301, 2},
      {0x0309, 3},
      {0x0303, 4},
      {0x0323, 5},
  };

  static std::map<char32_t, const char32_t *> build_row_index()
  {
    std::map<char32_t, const char32_t *> t;

    for (const char32_t *row : kToneRows)
    {
      t[row[0]] = row;
    }

    return t;
  }

  const std::map<char32_t, const char32_t *> &row_index()
  {
    static const std::map<char32_t, const char32_t *> m = build_row_index();
    return m;
  }

  bool is_diacritic_mark(char32_t c)
  {
    return c == 0x0302 || c == 0x0306 || c == 0x031B;
  }

  bool is_tone_mark(char32_t c)
  {
    auto result = kToneCol.find(c);

    return result != kToneCol.end();
  }

  char32_t apply_tone(char32_t letter, char32_t mark)
  {
    auto row = row_index().find(letter);
    auto tone_col = kToneCol.find(mark);

    if (row != row_index().end() && tone_col != kToneCol.end())
    {
      return row->second[tone_col->second];
    }
    else
    {
      return 0;
    }
  }
}

std::string to_nfc(const std::string &s)
{
  const std::u32string in = utf8_to_utf32(s);
  std::u32string out;

  size_t i = 0;
  while (i < in.size())
  {
    char32_t base = in[i++];

    char32_t diacritic = 0;
    char32_t tone = 0;

    while (i < in.size() && (is_diacritic_mark(in[i]) || is_tone_mark(in[i])))
    {
      if (is_diacritic_mark(in[i]))
      {
        diacritic = in[i];
      }
      else if (is_tone_mark(in[i]))
      {
        tone = in[i];
      }

      ++i;
    }

    if (diacritic != 0)
    {
      auto get_diacritic = kDiacritic.find({base, diacritic});
      if (get_diacritic != kDiacritic.end())
      {
        base = get_diacritic->second;
        diacritic = 0;
      }
    }

    if (tone != 0)
    {
      char32_t base_with_tone = apply_tone(base, tone);

      if (base_with_tone != 0)
      {
        base = base_with_tone;
        tone = 0;
      }
    }

    out.push_back(base);

    if (tone != 0)
    {
      out.push_back(tone);
    }

    if (diacritic != 0)
    {
      out.push_back(diacritic);
    }
  }

  return utf32_to_utf8(out);
}

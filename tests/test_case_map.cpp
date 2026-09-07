#include "case_map.h"
#include "case_table.h"
#include "check.h"

#include <string>

using vietime::is_upper_ascii;
using vietime::to_lower_ascii;
using vietime::to_upper_viet;

//

static std::string cp_name(const char *what, char32_t c)
{
  char buf[32];
  std::snprintf(buf, sizeof buf, "%s U+%04X", what, static_cast<unsigned>(c));
  return buf;
}

static void upper_matches_unicode_table()
{
  for (int i = 0; i < kCaseTableSize; ++i)
  {
    const CasePair &p = kCaseTable[i];
    check_char32(to_upper_viet(p.lower), p.upper, cp_name("to_upper_viet", p.lower));
  }
}

static void upper_leaves_everything_else_alone()
{
  const char32_t untouched[] = {
      U' ',
      U'\t',
      U'\n', // khoảng trắng
      U'0',
      U'2',
      U'9', // chữ số
      U'-',
      U'.',
      U',',
      U'!',
      U'@', // dấu câu
      U'A',
      U'M',
      U'Z',      // ASCII đã hoa
      U'\u00C2', // Â — đã hoa
      U'\u1EC6', // Ệ — đã hoa
      U'\u0102', // Ă — đã hoa, codepoint chẵn
      U'\u00F7', // ÷ — Latin-1 nhưng không phải chữ
      U'\u4E2D', // 中 — ngoài mọi khoảng ta quan tâm
  };

  for (char32_t c : untouched)
    check_char32(to_upper_viet(c), c, cp_name("giu nguyen", c));
}

static void upper_is_idempotent()
{
  for (int i = 0; i < kCaseTableSize; ++i)
  {
    const char32_t once = to_upper_viet(kCaseTable[i].lower);
    check_char32(to_upper_viet(once), once, cp_name("bat bien khi lap", once));
  }
}

static void ascii_pair_is_consistent()
{
  for (int i = 0; i < 128; ++i)
  {
    const char c = static_cast<char>(i);
    const bool changed = (to_lower_ascii(c) != c);
    check_eq(is_upper_ascii(c), changed,
             cp_name("cap ascii nhat quan", static_cast<char32_t>(i)));
  }

  check_eq(to_lower_ascii('A'), 'a', "to_lower_ascii('A')");
  check_eq(to_lower_ascii('Z'), 'z', "to_lower_ascii('Z')");
  check_eq(to_lower_ascii('a'), 'a', "to_lower_ascii('a') khong doi");
  check_eq(to_lower_ascii('5'), '5', "to_lower_ascii('5') khong doi");
  check_eq(to_lower_ascii('['), '[', "to_lower_ascii('[') khong doi");
}

void run_case_map_tests()
{
  upper_matches_unicode_table();
  upper_leaves_everything_else_alone();
  upper_is_idempotent();
  ascii_pair_is_consistent();
}
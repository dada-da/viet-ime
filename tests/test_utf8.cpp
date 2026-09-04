#include "check.h"
#include "utf8.h"

void test_utf8()
{
  // utf8_char_len
  check(utf8_char_len(0x41) == 1, "char_len: ASCII");         // A
  check(utf8_char_len(0xC3) == 2, "char_len: 2 bytes");       // Ã
  check(utf8_char_len(0xE1) == 3, "char_len: 3 byte");        // á
  check(utf8_char_len(0xF0) == 4, "char_len: 4 byte");        // ð
  check(utf8_char_len(0x80) == 0, "char_len: byte tiep noi"); // €
  check(utf8_char_len(0xFF) == 0, "char_len: byte rac");      // ÿ

  //  utf8_prev_boundary
  // "tiếng" = 7 byte:
  //   [0] 74 't'   [1] 69 'i'   [2] E1 ─┐
  //   [3] BA       [4] BF       ────────┴─ 'ế'
  //   [5] 6E 'n'   [6] 67 'g'
  const std::string s = "ti\u1ebfng";
  check(utf8_prev_boundary(s, 0) == 0, "prev_boundary: pos 0 khong underflow");
  check(utf8_prev_boundary(s, 1) == 0, "prev_boundary: lui ve dau chuoi");
  check(utf8_prev_boundary(s, 7) == 6, "prev_boundary: lui qua chu 1 byte");
  check(utf8_prev_boundary(s, 5) == 2, "prev_boundary: lui qua chu 3 byte");

  // utf_char_count
  check(utf8_char_count("") == 0, "char_count: rong");
  check(utf8_char_count(s) == 5, "char_count: tiếng 5 char chu khong phai 7");
  check(utf8_char_count("ð") == 1, "char_count: 1 (4 bytes)");
  check(utf8_char_count("Chuỗi tiếng việt") == 16, "char_count: 1 cau tieng viet 14 chu 2 dau space");

  // utf_is_valid
  check(!utf8_is_valid("\xE1\xBA"), "utf8_is_valid: cut");
  check(!utf8_is_valid("\x80"), "utf8_is_valid: byte tiep noi mo coi");
  check(!utf8_is_valid("\xE1\x41\x41"), "utf8_is_valid: thieu byte tiep noi");
  check(utf8_is_valid("\xe1\xba\xa1"), "utf8_is_valid: ạ trong UTF-8");
  check(utf8_is_valid("\x5A\x6F\xC3\xAB"), "utf8_is_valid: 'Zoë' trong UTF-8");

  // round-trip
  const std::string sample =
      "Tiếng Việt là ngôn ngữ chính thức của nước Cộng hoà "
      "Xã hội Chủ nghĩa Việt Nam cho tới ngày nay";

  const std::string space_sample = "                    ";
  check_str(utf32_to_utf8(utf8_to_utf32(sample)), sample, "round-trip: 1 cau dai tieng viet (21 tu)");
  check(utf8_to_utf32(sample).size() == utf8_char_count(sample), "utf8_char_count: kiem tra cheo size so voi utf8_char_count");
  check_str(utf32_to_utf8(utf8_to_utf32(space_sample)), space_sample, "round-trip: 1 string chi co space");
  check_str(utf32_to_utf8(utf8_to_utf32("A")), "A", "round-trip: A");
  check_str(utf32_to_utf8(utf8_to_utf32("\u00e9")), "\u00e9", "round-trip: \u00e9");
  check_str(utf32_to_utf8(utf8_to_utf32("\u20ac")), "\u20ac", "round-trip: \u20ac");
  check_str(utf32_to_utf8(utf8_to_utf32("\U0001F600")), "\U0001F600", "round-trip: \U0001F600");

  // utf8_to_utf32
  check(utf8_to_utf32("\u1ec1")[0] == 0x1EC1, "utf8_to_utf32: ê");
  check(utf8_to_utf32("😀")[0] == 0x1F600, "utf8_to_utf32: 😀 emoji");
  check(utf8_to_utf32("�")[0] == 0xFFFD, "utf8_to_utf32: � byte hong");
}
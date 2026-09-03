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
}
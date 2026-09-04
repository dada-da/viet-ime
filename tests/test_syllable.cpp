#include "syllable.h"
#include "normalize.h"
#include "utf8.h"

#include <cstddef>
#include <iostream>
#include <string>

using namespace vietime;

static int g_failed = 0;
static int g_total = 0;

// So sánh vị trí đặt dấu. Đổi tên/thân hàm cho khớp harness của bạn.
static void check_pos(const std::u32string &input,
                      std::size_t expected,
                      const char *rule)
{
  ++g_total;
  const std::size_t actual = find_tone_position(input);
  if (actual == expected)
    return;

  ++g_failed;
  std::cout << "  FAIL [" << rule << "] \"" << utf32_to_utf8(input) << "\"\n"
            << "        expected = " << static_cast<long long>(expected)
            << ", actual = " << static_cast<long long>(actual) << "\n";
}

static void check_parts(const std::u32string &input,
                        const std::u32string &initial,
                        const std::u32string &nucleus,
                        const std::u32string &coda)
{
  ++g_total;
  const SyllableParts p = split_syllable(input);
  if (p.initial == initial && p.nucleus == nucleus && p.coda == coda)
    return;

  ++g_failed;
  std::cout << "  FAIL split \"" << utf32_to_utf8(input) << "\"\n"
            << "        expected = "
            << utf32_to_utf8(initial) << " | "
            << utf32_to_utf8(nucleus) << " | "
            << utf32_to_utf8(coda) << "\n"
            << "        actual   = "
            << utf32_to_utf8(p.initial) << " | "
            << utf32_to_utf8(p.nucleus) << " | "
            << utf32_to_utf8(p.coda) << "\n";
}

int main()
{
  std::cout << "== split_syllable ==\n";
  check_parts(U"tiêu", U"t", U"iêu", U"");
  check_parts(U"quan", U"qu", U"a", U"n");
  check_parts(U"giương", U"gi", U"ươ", U"ng");
  check_parts(U"nghiêng", U"ngh", U"iê", U"ng"); // 'h' sát nguyên âm, không phải 'g'
  check_parts(U"gi", U"g", U"i", U"");           // không cắt: âm chính sẽ rỗng
  check_parts(U"ăn", U"", U"ă", U"n");           // không có âm đầu → chốt i > 0
  check_parts(U"ng", U"ng", U"", U"");
  check_parts(U"", U"", U"", U"");

  std::cout << "== luật 1: ơ ==\n";
  check_pos(U"ngươi", 3, "R1");
  check_pos(U"đươc", 2, "R1");
  check_pos(U"giương", 3, "R1"); // ngoại lệ gi + luật ơ cùng lúc

  std::cout << "== luật 2: ê ==\n";
  check_pos(U"tiêng", 2, "R2");
  check_pos(U"tiêu", 2, "R2");
  check_pos(U"nghiêng", 4, "R2");
  check_pos(U"quyên", 3, "R2"); // qu cắt trước, âm chính "yê"

  std::cout << "== luật 3: ă â ô ư ==\n";
  check_pos(U"cưa", 1, "R3");
  check_pos(U"tuôi", 2, "R3");
  check_pos(U"uông", 1, "R3");
  check_pos(U"ăn", 0, "R3"); // vị trí 0 hợp lệ, đừng nhầm với NO_TONE_POS

  std::cout << "== luật 4: một nguyên âm ==\n";
  check_pos(U"ta", 1, "R4");
  check_pos(U"qua", 2, "R4");
  check_pos(U"gia", 2, "R4");
  check_pos(U"quy", 2, "R4");
  check_pos(U"gi", 1, "R4");

  std::cout << "== luật 5: có âm cuối → nguyên âm cuối ==\n";
  check_pos(U"toan", 2, "R5");
  check_pos(U"hoan", 2, "R5");
  check_pos(U"oanh", 1, "R5");

  std::cout << "== luật 6: 3 nguyên âm, không âm cuối → giữa ==\n";
  check_pos(U"ngoai", 3, "R6");

  std::cout << "== luật 7: còn lại → nguyên âm đầu ==\n";
  check_pos(U"cua", 1, "R7");
  check_pos(U"hoa", 1, "R7");
  check_pos(U"chia", 2, "R7");

  std::cout << "== không có nguyên âm ==\n";
  check_pos(U"ng", NO_TONE_POS, "none");
  check_pos(U"", NO_TONE_POS, "none");

  std::cout << "\n"
            << (g_total - g_failed) << "/" << g_total << " passed\n";
  return g_failed == 0 ? 0 : 1;
}
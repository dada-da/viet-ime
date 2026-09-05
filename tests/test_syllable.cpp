#include "check.h"
#include "syllable.h"
#include "utf8.h"

#include <cstddef>
#include <string>

using namespace vietime;

namespace
{

  std::string pos_str(std::size_t p)
  {
    return std::to_string(static_cast<long long>(p));
  }

  void check_pos(const std::u32string &input, std::size_t want, const std::string &rule)
  {
    const std::size_t got = find_tone_position(input);
    const std::string name = "[" + rule + "] " + utf32_to_utf8(input);

    check(got == want, name);
    if (got != want)
    {
      std::cout << "    want: " << pos_str(want) << "\n"
                << "    got:  " << pos_str(got) << "\n";
    }
  }

  void check_parts(const std::u32string &input,
                   const std::u32string &initial,
                   const std::u32string &nucleus,
                   const std::u32string &coda)
  {
    const SyllableParts p = split_syllable(input);
    const std::string name = "split \"" + utf32_to_utf8(input) + "\"";

    const bool ok = (p.initial == initial && p.nucleus == nucleus && p.coda == coda);
    check(ok, name);
    if (!ok)
    {
      std::cout << "    want: " << utf32_to_utf8(initial)
                << " | " << utf32_to_utf8(nucleus)
                << " | " << utf32_to_utf8(coda) << "\n"
                << "    got:  " << utf32_to_utf8(p.initial)
                << " | " << utf32_to_utf8(p.nucleus)
                << " | " << utf32_to_utf8(p.coda) << "\n";
    }

    // nucleus_start phai tro dung vao vi tri nucleus trong chuoi goc
    check(input.compare(p.nucleus_start, p.nucleus.size(), p.nucleus) == 0,
          name + " (nucleus_start)");
  }

} // namespace

void run_syllable_tests()
{
  // --- split_syllable ---
  check_parts(U"tiêu", U"t", U"iêu", U"");
  check_parts(U"quan", U"qu", U"a", U"n");
  check_parts(U"giương", U"gi", U"ươ", U"ng");
  check_parts(U"nghiêng", U"ngh", U"iê", U"ng"); // 'h' sat nguyen am, khong phai 'g'
  check_parts(U"gi", U"g", U"i", U"");           // khong cat: am chinh se rong
  check_parts(U"ăn", U"", U"ă", U"n");           // khong co am dau -> chot i > 0
  check_parts(U"ng", U"ng", U"", U"");
  check_parts(U"", U"", U"", U"");

  // --- luat 1: o-horn ---
  check_pos(U"ngươi", 3, "R1");
  check_pos(U"đươc", 2, "R1");
  check_pos(U"giương", 3, "R1"); // ngoai le gi + luat o-horn cung luc

  // --- luat 2: e-circ ---
  check_pos(U"tiêng", 2, "R2");
  check_pos(U"tiêu", 2, "R2");
  check_pos(U"nghiêng", 4, "R2");
  check_pos(U"quyên", 3, "R2"); // qu cat truoc, am chinh "ye-circ"

  // --- luat 3: a-breve a-circ o-circ u-horn ---
  check_pos(U"cưa", 1, "R3");
  check_pos(U"tuôi", 2, "R3");
  check_pos(U"uông", 1, "R3");
  check_pos(U"ăn", 0, "R3"); // vi tri 0 hop le, dung nham voi NO_TONE_POS

  // --- luat 4: mot nguyen am ---
  check_pos(U"ta", 1, "R4");
  check_pos(U"qua", 2, "R4");
  check_pos(U"gia", 2, "R4");
  check_pos(U"quy", 2, "R4");
  check_pos(U"gi", 1, "R4");

  // --- luat 5: co am cuoi -> nguyen am cuoi ---
  check_pos(U"toan", 2, "R5");
  check_pos(U"hoan", 2, "R5");
  check_pos(U"oanh", 1, "R5");

  // --- luat 6: 3 nguyen am, khong am cuoi -> giua ---
  check_pos(U"ngoai", 3, "R6");

  // --- luat 7: con lai -> nguyen am dau ---
  check_pos(U"cua", 1, "R7");
  check_pos(U"hoa", 1, "R7");
  check_pos(U"chia", 2, "R7");

  // --- khong co nguyen am ---
  check_pos(U"ng", NO_TONE_POS, "none");
  check_pos(U"", NO_TONE_POS, "none");
}
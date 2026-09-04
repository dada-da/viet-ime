#include "check.h"
#include "tone_table.h"
#include "utf8.h"
#include "normalize.h"

void run_tone_table_tests()
{
  check_eq(tone_from_telex('s'), TONE_SAC, "telex s = sac");
  check_eq(tone_from_telex('f'), TONE_HUYEN, "telex f = huyen");
  check_eq(tone_from_telex('r'), TONE_HOI, "telex r = hoi");
  check_eq(tone_from_telex('x'), TONE_NGA, "telex x = nga");
  check_eq(tone_from_telex('j'), TONE_NANG, "telex j = nang");
  check_eq(tone_from_telex('a'), TONE_NONE, "telex a khong phai thanh");
  check_eq(tone_from_telex('S'), TONE_NONE, "telex hoa chua ho tro");
  check_eq(tone_from_telex('1'), TONE_NONE, "phim VNI khong lot vao telex");

  check_eq(tone_from_vni('1'), TONE_SAC, "vni 1 = sac");
  check_eq(tone_from_vni('5'), TONE_NANG, "vni 5 = nang");
  check_eq(tone_from_vni('6'), TONE_NONE, "vni 6 la bien doi");
  check_eq(tone_from_vni('s'), TONE_NONE, "phim telex khong lot vao vni");

  check_char32(apply_tone_to_vowel(U'a', TONE_SAC), U'á', "a + sac");
  check_char32(apply_tone_to_vowel(U'a', TONE_HUYEN), U'à', "a + huyen");
  check_char32(apply_tone_to_vowel(U'a', TONE_HOI), U'ả', "a + hoi");
  check_char32(apply_tone_to_vowel(U'a', TONE_NGA), U'ã', "a + nga");
  check_char32(apply_tone_to_vowel(U'a', TONE_NANG), U'ạ', "a + nang");

  check_char32(apply_tone_to_vowel(U'ă', TONE_SAC), U'ắ', "a-breve + sac");
  check_char32(apply_tone_to_vowel(U'ă', TONE_NANG), U'ặ', "a-breve + nang");
  check_char32(apply_tone_to_vowel(U'â', TONE_HUYEN), U'ầ', "a-circ + huyen");
  check_char32(apply_tone_to_vowel(U'â', TONE_NGA), U'ẫ', "a-circ + nga");
  check_char32(apply_tone_to_vowel(U'ê', TONE_SAC), U'ế', "e-circ + sac (tieng)");
  check_char32(apply_tone_to_vowel(U'ê', TONE_HOI), U'ể', "e-circ + hoi");
  check_char32(apply_tone_to_vowel(U'ô', TONE_HOI), U'ổ', "o-circ + hoi (tuoi)");
  check_char32(apply_tone_to_vowel(U'ô', TONE_NANG), U'ộ', "o-circ + nang");
  check_char32(apply_tone_to_vowel(U'ơ', TONE_HUYEN), U'ờ', "o-horn + huyen (nguoi)");
  check_char32(apply_tone_to_vowel(U'ơ', TONE_NANG), U'ợ', "o-horn + nang (duoc)");
  check_char32(apply_tone_to_vowel(U'ư', TONE_SAC), U'ứ', "u-horn + sac");
  check_char32(apply_tone_to_vowel(U'ư', TONE_NGA), U'ữ', "u-horn + nga");

  check_char32(apply_tone_to_vowel(U'e', TONE_NANG), U'ẹ', "e + nang");
  check_char32(apply_tone_to_vowel(U'i', TONE_NGA), U'ĩ', "i + nga");
  check_char32(apply_tone_to_vowel(U'o', TONE_HOI), U'ỏ', "o + hoi");
  check_char32(apply_tone_to_vowel(U'u', TONE_HUYEN), U'ù', "u + huyen");
  check_char32(apply_tone_to_vowel(U'y', TONE_SAC), U'ý', "y + sac");

  check_char32(apply_tone_to_vowel(U'a', TONE_NONE), U'a', "TONE_NONE tra nguyen");
  check_char32(apply_tone_to_vowel(U'n', TONE_SAC), U'n', "phu am miss bang");
  check_char32(apply_tone_to_vowel(U'đ', TONE_SAC), U'đ', "d-stroke khong mang thanh");
  check_char32(apply_tone_to_vowel(U'A', TONE_SAC), U'A', "hoa chua ho tro");
  check_char32(apply_tone_to_vowel(U'á', TONE_HUYEN), U'á', "da co thanh: miss bang");
  check_char32(apply_tone_to_vowel(U' ', TONE_SAC), U' ', "khoang trang");

  check_str(utf32_to_utf8(std::u32string(1, apply_tone_to_vowel(U'ơ', TONE_HUYEN))),
            to_nfc("ơ\u0300"), "tone_table khop to_nfc: o-horn huyen");
  check_str(utf32_to_utf8(std::u32string(1, apply_tone_to_vowel(U'ê', TONE_SAC))),
            to_nfc("ê\u0301"), "tone_table khop to_nfc: e-circ sac");
}
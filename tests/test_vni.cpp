// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "typing.h"

static void vni_four_modifiers()
{
  check_vni("a6", "â");
  check_vni("e6", "ê");
  check_vni("o6", "ô");

  check_vni("o7", "ơ");
  check_vni("u7", "ư");

  check_vni("a8", "ă");

  check_vni("d9", "đ");
}

static void vni_modifiers_do_not_overlap()
{
  check_vni("a7", "a7");
  check_vni("e7", "e7");
  check_vni("i7", "i7");

  check_vni("o8", "o8");
  check_vni("u8", "u8");
  check_vni("e8", "e8");

  check_vni("i6", "i6");
  check_vni("u6", "u6");
  check_vni("y6", "y6");

  check_vni("a9", "a9");
  check_vni("o9", "o9");
  check_vni("a82", "ằ");
}

static void vni_uo_pair()
{
  check_vni("uo7", "ươ");
  check_vni("ngu7o7i2", "người");
  check_vni("nguo7i2", "người");
  check_vni("d9u7o7c5", "được");
  check_vni("d9uo7c5", "được");
  check_vni("uo8", "uo8");
}

static void vni_five_tones()
{
  check_vni("a1", "á");
  check_vni("a2", "à");
  check_vni("a3", "ả");
  check_vni("a4", "ã");
  check_vni("a5", "ạ");
}

static void vni_digit_passthrough()
{
  check_vni("2024", "2024");
  check_vni("1", "1");
  check_vni("123", "123");
  check_vni("0", "0");
  check_vni("abc", "abc");
}

static void vni_telex_keys_are_literal()
{
  check_vni("aa", "aa");
  check_vni("ee", "ee");
  check_vni("oo", "oo");
  check_vni("aw", "aw");
  check_vni("uw", "uw");
  check_vni("dd", "dd");
  check_vni("as", "as");
  check_vni("af", "af");
}

static void vni_real_words()
{
  check_vni("tie6ng1", "tiếng");
  check_vni("vie6t5", "việt");
  check_vni("d9o6ng2", "đồng");
  check_vni("d9a6y5", "đậy");
  check_vni("ba8ng2", "bằng");
  check_vni("tuo6i3", "tuổi");
  check_vni("qua1", "quá");
  check_vni("ho6m2", "hồm");
  check_vni("duo7ng2", "dường");
  check_vni("hoc5", "học");
  check_vni("nam2024", "nãm");
  check_vni("a888", "a88");
  check_vni("o776", "o76");
}

static void vni_tone_placement()
{
  check_vni_tone_placement("hoa2", "hòa", vietime::PLACEMENT_CLASSIC);
  check_vni_tone_placement("hoa2", "hoà", vietime::PLACEMENT_MODERN);
  check_vni_tone_placement("thuy3", "thủy", vietime::PLACEMENT_CLASSIC);
  check_vni_tone_placement("thuy3", "thuỷ", vietime::PLACEMENT_MODERN);
}

void run_vni_tests()
{
  vni_four_modifiers();
  vni_modifiers_do_not_overlap();
  vni_uo_pair();
  vni_five_tones();
  vni_digit_passthrough();
  vni_telex_keys_are_literal();
  vni_real_words();
  vni_tone_placement();
}
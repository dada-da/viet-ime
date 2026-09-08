// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "check.h"
#include "typing.h"

namespace
{
  using namespace vietime;

  void test_undo_modifier_basic()
  {
    check_telex("aaa", "aa");
    check_telex("eee", "ee");
    check_telex("ooo", "oo");
    check_telex("aww", "aw");
    check_telex("uww", "uw");
    check_telex("oww", "ow");
  }

  void test_undo_dd()
  {
    check_telex("ddd", "dd");
    check_telex("ddda", "dda");
  }

  void test_undo_tone()
  {
    check_telex("ass", "as");
    check_telex("aff", "af");
    check_telex("arr", "ar");
    check_telex("axx", "ax");
    check_telex("ajj", "aj");
    check_telex("tieengss", "tiêngs");
    check_telex("aass", "âs");
  }

  void test_undo_nucleus_offset()
  {
    check_telex("tieenge", "tienge");
    check_telex("thoongo", "thongo");
  }

  void test_undo_pair_uo()
  {
    check_telex("nguoiww", "nguoiw");
    check_telex("duowcw", "duocw");
  }

  void test_undo_across_literal()
  {
    check_telex("nguowiw", "nguoiw");
    check_telex("asbs", "abs");
    check_telex("aaba", "aba");
  }

  void test_undo_uppercase()
  {
    check_telex("AAA", "AA");
    check_telex("aAA", "aA");
    check_telex("AAa", "Aa");
    check_telex("DDD", "DD");
  }

  void test_undo_accepted_limits()
  {
    check_telex("theses", "thês");
  }

  void test_tone_removal_telex()
  {
    check_telex("nafz", "na");
    check_telex("tieengsz", "tiêng");
    check_telex("nguoiwfz", "ngươi");
    check_telex("az", "az");
    check_telex("z", "z");
    check_telex("nafzs", "ná");
  }

  void test_tone_removal_vni()
  {
    check_vni("nam2024", "nãm");
    check_vni("na20", "na");
    check_vni("a0", "a0");
    check_vni("tie6ng10", "tiêng");
  }

  void test_no_regression()
  {
    check_vni("2024", "2024");
    check_telex("sss", "sss");
    check_telex("tieengs", "tiếng");
    check_telex("nguoiwf", "người");
    check_telex("buoonf", "buồn");
    check_telex("dduocwj", "được");
  }

  void test_backspace_clears_last()
  {
    KeyProcessor kp;
    kp.handle_key('a');
    kp.handle_key('a');
    kp.backspace();
    kp.handle_key('a');
    check_str("backspace xoa last_", kp.preedit(), "a");

    KeyProcessor kp2;
    kp2.handle_key('a');
    kp2.handle_key('a');
    kp2.reset();
    kp2.handle_key('a');
    check_str("reset xoa last_", kp2.preedit(), "a");
  }
} // namespace

void test_undo()
{
  test_undo_modifier_basic();
  test_undo_dd();
  test_undo_tone();
  test_undo_nucleus_offset();
  test_undo_pair_uo();
  test_undo_across_literal();
  test_undo_uppercase();
  test_undo_accepted_limits();
  test_tone_removal_telex();
  test_tone_removal_vni();
  test_no_regression();
  test_backspace_clears_last();
}
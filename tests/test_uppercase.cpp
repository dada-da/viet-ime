// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "typing.h"

using vietime::KeyProcessor;
using vietime::METHOD_TELEX;
using vietime::METHOD_VNI;

static void upper_initial_consonant()
{
  check_telex("Vieejt", "Việt");
  check_telex("Nguwowif", "Người");
  check_telex("Hoaf", "Hòa");
  check_vni("Vie6t5", "Việt");
  check_vni("Hoa2", "Hòa");
}

static void upper_on_modified_vowel()
{
  check_telex("AAn", "Ân");
  check_telex("OOng", "Ông");
  check_telex("AWn", "Ăn");
  check_telex("Uwng", "Ưng");
  check_vni("A6n", "Ân");
  check_vni("O6ng", "Ông");
  check_vni("A8n", "Ăn");
}

static void upper_d_stroke()
{
  check_telex("DDoongf", "Đồng");
  check_telex("DDi", "Đi");
  check_vni("D9o6ng2", "Đồng");
  check_vni("D9i", "Đi");
}

static void upper_keys_still_work()
{
  check_telex("tieengS", "tiếng");
  check_telex("nguoiWf", "người");
  check_telex("nguoiwF", "người");
  check_telex("dduocWJ", "được");
  check_vni("tie6ng1", "tiếng");
}

static void upper_whole_word()
{
  check_telex("HAF", "HÀ");
  check_telex("VIEEJT", "VIỆT");
  check_telex("DDOONGF", "ĐỒNG");
  check_telex("NGUWOWIF", "NGƯỜI");
  check_vni("HA2", "HÀ");
  check_vni("VIE6T5", "VIỆT");
  check_vni("NGU7O7I2", "NGƯỜI");
}

static void upper_mask_is_positional()
{
  check_telex("aAn", "ân");
  check_telex("Aan", "Ân");
}

static void upper_backspace_and_reset()
{
  {
    KeyProcessor kp;
    kp.handle_key('H');
    kp.handle_key('a');
    check_str(kp.preedit(), "Ha", "backspace: truoc khi xoa");
    kp.backspace();
    check_str(kp.preedit(), "H", "backspace: mat na co lai dung");
    kp.handle_key('o');
    check_str(kp.preedit(), "Ho", "backspace: go tiep sau khi xoa");
  }

  {

    KeyProcessor kp;
    kp.handle_key('H');
    kp.backspace();
    kp.handle_key('a');
    check_str(kp.preedit(), "a", "backspace ve rong roi go lai");
  }

  {

    KeyProcessor kp;
    kp.handle_key('H');
    kp.handle_key('A');
    kp.reset();
    kp.handle_key('a');
    kp.handle_key('n');
    check_str(kp.preedit(), "an", "reset xoa ca mat na");
  }

  {

    KeyProcessor kp;
    check_eq(kp.backspace(), false, "backspace tren buffer rong tra false");
    check_str(kp.preedit(), "", "backspace tren buffer rong khong doi gi");
  }
}

static void switch_method_at_runtime()
{
  KeyProcessor kp;

  kp.handle_key('a');
  kp.handle_key('s');
  check_str(kp.preedit(), "á", "switch: telex truoc khi doi");

  kp.reset();
  kp.set_method(METHOD_VNI);
  kp.handle_key('a');
  kp.handle_key('1');
  check_str(kp.preedit(), "á", "switch: vni sau khi doi");

  kp.reset();
  kp.set_method(METHOD_TELEX);
  kp.handle_key('a');
  kp.handle_key('1');
  check_str(kp.preedit(), "a1", "switch: quay lai telex, 1 la literal");
}

void run_uppercase_tests()
{
  upper_initial_consonant();
  upper_on_modified_vowel();
  upper_d_stroke();
  upper_keys_still_work();
  upper_whole_word();
  upper_mask_is_positional();
  upper_backspace_and_reset();
  switch_method_at_runtime();
}
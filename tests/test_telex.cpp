// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "typing.h"

static void telex_seven_rules()
{
  check_telex("aa", "â");
  check_telex("ee", "ê");
  check_telex("oo", "ô");
  check_telex("aw", "ă");
  check_telex("ow", "ơ");
  check_telex("uw", "ư");
  check_telex("dd", "đ");
}

static void telex_rules_in_context()
{
  check_telex("caan", "cân");
  check_telex("been", "bên");
  check_telex("boong", "bông");
  check_telex("bawng", "băng");
  check_telex("bown", "bơn");
  check_telex("bung", "bung");
  check_telex("buwng", "bưng");
  check_telex("ddan", "đan");
}

static void telex_five_tones()
{
  check_telex("as", "á");
  check_telex("af", "à");
  check_telex("ar", "ả");
  check_telex("ax", "ã");
  check_telex("aj", "ạ");
}

static void telex_tone_on_modified_vowel()
{
  check_telex("aas", "ấ");
  check_telex("aws", "ắ");
  check_telex("ees", "ế");
  check_telex("oos", "ố");
  check_telex("ows", "ớ");
  check_telex("uws", "ứ");
}

static void telex_uo_pair()
{
  check_telex("uow", "ươ");
  check_telex("nguow", "ngươ");
  check_telex("dduocwj", "được");
  check_telex("dduwowcj", "được");
  check_telex("khoong", "không");
  check_telex("quoocs", "quốc");
  check_telex("buoonf", "buồn");
}

static void telex_passthrough()
{
  check_telex("ew", "ew");
  check_telex("iw", "iw");
  check_telex("yw", "yw");
  check_telex("2024", "2024");
  check_telex("a6", "a6");
  check_telex("a7", "a7");
  check_telex("a8", "a8");
  check_telex("a1", "a1");
  check_telex("dd9", "đ9");
  check_telex("sss", "sss");
  check_telex("", "");
}

static void telex_tone_toggle()
{
  check_telex("toanss", "toan");
  check_telex("toansj", "toạn");
  check_telex("assf", "à");
}

void run_telex_tests()
{
  telex_seven_rules();
  telex_rules_in_context();
  telex_five_tones();
  telex_tone_on_modified_vowel();
  telex_uo_pair();
  telex_passthrough();
  telex_tone_toggle();
}
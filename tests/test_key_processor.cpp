// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <string>
#include "key_processor.h"
#include "check.h"
#include "utf8.h"

using namespace vietime;

static void check_typing(const char *keys, const std::string &want)
{
  vietime::KeyProcessor kp;
  std::string out;

  for (const char *k = keys; *k; ++k)
  {
    if (*k == '~') // for test
    {
      kp.backspace();
      continue;
    }

    vietime::KeyResult result = kp.handle_key(*k);

    if (result.has_commit && result.consumed)
    {
      out.append(result.commit_text);
      continue;
    }
  }

  out += kp.commit();
  check_str(out, want, std::string("go \"") + keys + "\"");
}

static void check_char_count(const char *keys)
{
  vietime::KeyProcessor kp;
  std::string out;

  for (const char *k = keys; *k; ++k)
  {

    vietime::KeyResult result = kp.handle_key(*k);

    if (result.has_commit && result.consumed)
    {
      out.append(result.commit_text);
      continue;
    }

    std::string msg;
    msg += "char_count - utf8_char_count: ";
    msg += std::string(1, *k);

    check(kp.char_count() == utf8_char_count(kp.preedit()), msg);
  }

  out += kp.commit();
  check(kp.char_count() == utf8_char_count(kp.preedit()), "char_count - utf8_char_count: " + out);
}

void run_pipeline_tests()
{
  check_typing("nguwowif", "người");
  check_typing("nguoiwf", "người");
  check_typing("tieengs", "tiếng");
  check_typing("tienges", "tiếng");
  check_typing("toans", "toán");
  check_typing("tosan", "toán"); // thanh gõ giữa chừng
  check_typing("hoaf", "hòa");
  check_typing("cuar", "của");
  check_typing("quas", "quá"); // không được ra "qúa"
  check_typing("dduwowcj", "được");
  check_typing("dduocwj", "được");
  check_typing("tuooir", "tuổi");
  check_typing("toansj", "toạn");  // thanh mới đè thanh cũ
  check_typing("toanss", "toans"); // gõ lại = bỏ dấu
  check_typing("ngoaif", "ngoài");
  check_typing("khoongs", "khống"); // oo tìm ngược, không đụng luật cặp
  check_typing("quoocs", "quốc");   // ngoại lệ qu: u ở âm đầu, oo áp lên o
  check_typing("", "");
  check_typing("tienges vieejt.", "tiếng việt.");
  check_typing("caf phee", "cà phê");
  check_typing("tienges~", "tiến");
  check_typing("hojc~w", "hợ");
  check_typing("quanww~w", "quăn");
  check_typing("quanwwb~w", "quanww");
}

void run_char_count_tests()
{
  check_char_count("");
  check_char_count("a");
  check_char_count("tieengs");
  check_char_count("tieengs ");
  check_char_count("NGUwOwIF");
  check_char_count("dduwowcj");
  check_char_count("hoaf");
}
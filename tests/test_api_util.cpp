// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "check.h"
#include "api_util.h"

#include <string>

using vietime::copy_text;

namespace
{
  struct Out
  {
    char text[VIETIME_MAX_TEXT_BYTES];
    std::size_t n;
  };

  Out run(const std::string &src)
  {
    Out out;
    // đổ rác vào buffer để bắt được trường hợp copy_text quên ghi NUL
    for (std::size_t i = 0; i < VIETIME_MAX_TEXT_BYTES; i++)
      out.text[i] = '\xAA';

    out.n = copy_text(out.text, src);
    return out;
  }

  std::string as_cstr(const Out &out) { return std::string(out.text); }

  std::string as_bytes(const Out &out) { return std::string(out.text, out.n); }
}

void test_api_util()
{
  {
    Out r = run("");
    check_eq<std::size_t>(r.n, 0, "copy_text: rỗng trả 0");
    check_eq<char>(r.text[0], '\0', "copy_text: rỗng ghi NUL ở ô 0");
    check_str(as_cstr(r), "", "copy_text: rỗng đọc ra chuỗi rỗng");
  }

  {
    Out r = run("a");
    check_eq<std::size_t>(r.n, 1, "copy_text: \"a\" trả 1");
    check_str(as_cstr(r), "a", "copy_text: \"a\" nguyên vẹn");
    check_eq<char>(r.text[1], '\0', "copy_text: \"a\" NUL ngay sau dữ liệu");
  }

  {
    Out r = run("chào");
    check_eq<std::size_t>(r.n, 5, "copy_text: \"chào\" là 5 byte");
    check_str(as_cstr(r), "chào", "copy_text: \"chào\" không mất byte cuối");
  }

  {
    Out r = run("Tiếng Việt");
    check_eq<std::size_t>(r.n, 14, "copy_text: \"Tiếng Việt\" là 14 byte");
    check_str(as_cstr(r), "Tiếng Việt", "copy_text: \"Tiếng Việt\" nguyên vẹn");
  }

  {
    std::string src(VIETIME_MAX_TEXT_BYTES - 1, 'x');
    Out r = run(src);
    check_eq<std::size_t>(r.n, VIETIME_MAX_TEXT_BYTES - 1, "copy_text: 63 byte chép đủ");
    check_str(as_cstr(r), src, "copy_text: 63 byte không cắt");
    check_eq<char>(r.text[VIETIME_MAX_TEXT_BYTES - 1], '\0',
                   "copy_text: 63 byte thì NUL nằm ở ô cuối cùng");
  }

  {
    std::string src(VIETIME_MAX_TEXT_BYTES, 'x'); // 64 ký tự
    Out r = run(src);
    check_eq<std::size_t>(r.n, VIETIME_MAX_TEXT_BYTES - 1, "copy_text: 64 byte cắt còn 63");
    check_eq<std::size_t>(as_cstr(r).size(), VIETIME_MAX_TEXT_BYTES - 1,
                          "copy_text: 64 byte vẫn kết thúc NUL");
  }

  {
    std::string src(200, 'y');
    Out r = run(src);
    check_eq<std::size_t>(r.n, VIETIME_MAX_TEXT_BYTES - 1, "copy_text: 200 byte cắt còn 63");
    check_eq<char>(r.text[VIETIME_MAX_TEXT_BYTES - 1], '\0',
                   "copy_text: 200 byte không ghi quá mảng");
  }

  {
    Out r = run("nghiêng");
    check_str(as_bytes(r), "nghiêng", "copy_text: n byte đầu đúng bằng nguồn");
  }

  {
    std::string src = "x";
    for (int i = 0; i < 21; i++)
      src += "ế";
    check_eq<std::size_t>(src.size(), 64, "ca 9: chuỗi nguồn đúng 64 byte");

    Out r = run(src);
    check_eq<std::size_t>(r.n, 63, "copy_text: cắt ở 63 byte");
    // 63 = 1 + 20*3 + 2  ->  ký tự 'ế' thứ 21 bị cắt còn 2 byte
    check_eq<unsigned char>(static_cast<unsigned char>(r.text[62]), 0xBA,
                            "copy_text: byte cuối là byte giữa của 'ế' (hành vi hiện tại)");
  }
}
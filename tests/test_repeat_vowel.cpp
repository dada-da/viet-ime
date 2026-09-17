// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

// Khoá hành vi khi gõ LẶP cùng một nguyên âm nhiều lần (Telex).
// Bug đã thấy: gõ 'a' lần 5 trên "aâ" biến nhầm chữ 'a' ĐẦU -> "ââ",
// đáng lẽ chữ cuối đã là 'â' rồi thì phải hoàn tác -> "aa".

#include "check.h"
#include "ime_api.h"

#include <string>

namespace
{
  // Gõ cả chuỗi phím rồi flush, trả chuỗi UTF-8 người dùng thấy.
  std::string type_telex(const std::string &keys)
  {
    vietime_ctx *ctx = vietime_create();
    vietime_set_method(ctx, VIETIME_METHOD_TELEX);

    std::string out;
    for (unsigned char c : keys)
    {
      VietimeKeyResult r = vietime_process_key(ctx, c);
      if (r.text_committed)
        out.append(r.text, r.text_length);
      // preedit chưa commit: bỏ qua, sẽ lấy ở flush
    }
    VietimeKeyResult f = vietime_flush(ctx);
    out.append(f.text, f.text_length);

    vietime_destroy(ctx);
    return out;
  }

  void ck(const std::string &keys, const std::string &want)
  {
    check_str(type_telex(keys), want, ("telex \"" + keys + "\"").c_str());
  }
}

void test_repeat_vowel()
{
  // ---- Ca cơ bản, ĐÃ đúng (làm mốc, không được vỡ) ----
  ck("aa", "\u00E2"); // â
  ck("aaa", "aa");    // hoàn tác lần 3
  ck("ee", "\u00EA"); // ê
  ck("eee", "ee");

  // ---- Ca bug: lặp dài, dấu áp cho chữ GẦN NHẤT ----
  ck("aaaa", "a\u00E2");    // aâ
  ck("eeee", "e\u00EA");    // eê
  ck("aaaaa", "aaa");       // aa -> gõ a nữa -> hoàn tác -> aa   ← BUG: đang ra "ââ"
  ck("eeeee", "eee");       // tương tự                          ← BUG: đang ra "êê"?
  ck("aaaaaa", "aa\u00E2"); // aaâ                               ← BUG: đang ra "âaa"
  ck("eeeeee", "ee\u00EA"); // eeê                               ← BUG: đang ra "êee"

  // ---- Ca trộn, ĐÃ đúng (không được vỡ khi sửa) ----
  ck("aeae", "aeae");         // âê
  ck("aaee", "\u00E2\u00EA"); // âê
  ck("eeoo", "\u00EA\u00F4"); // êô

  // ---- oaw: ĐIỀN SAU KHI CHẠY type_keys oaw ----
  ck("oaw", "o\u0103");
}
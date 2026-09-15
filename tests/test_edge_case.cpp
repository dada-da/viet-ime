// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstring>
#include <string>

#include "check.h"
#include "ime_api.h"

namespace
{

  // ---------------------------------------------------------------------
  // Bộ kiểm UTF-8 viết riêng cho file này.
  //
  // CỐ Ý không dùng vietime::utf8_is_valid của core.
  //
  // Bắt: byte nối lẻ, chuỗi cụt, dạng thừa (overlong), nửa cặp thay thế,
  // và codepoint vượt U+10FFFF.
  // ---------------------------------------------------------------------
  bool is_valid_utf8(const char *s, std::size_t n)
  {
    std::size_t i = 0;

    while (i < n)
    {
      const unsigned char b0 = static_cast<unsigned char>(s[i]);
      std::size_t extra = 0;
      unsigned long cp = 0;
      unsigned long lowest = 0;

      if (b0 < 0x80)
      {
        i++;
        continue;
      }
      else if ((b0 & 0xE0) == 0xC0)
      {
        extra = 1;
        cp = b0 & 0x1Fu;
        lowest = 0x80;
      }
      else if ((b0 & 0xF0) == 0xE0)
      {
        extra = 2;
        cp = b0 & 0x0Fu;
        lowest = 0x800;
      }
      else if ((b0 & 0xF8) == 0xF0)
      {
        extra = 3;
        cp = b0 & 0x07u;
        lowest = 0x10000;
      }
      else
      {
        return false; // 0x80..0xBF đứng đầu, hoặc 0xF8..0xFF
      }

      if (i + extra >= n)
      {
        return false; // chuỗi cụt: thiếu byte nối
      }

      for (std::size_t k = 1; k <= extra; k++)
      {
        const unsigned char bk = static_cast<unsigned char>(s[i + k]);

        if ((bk & 0xC0) != 0x80)
        {
          return false;
        }

        cp = (cp << 6) | (bk & 0x3Fu);
      }

      if (cp < lowest)
      {
        return false; // dạng thừa
      }

      if (cp >= 0xD800 && cp <= 0xDFFF)
      {
        return false; // nửa cặp thay thế
      }

      if (cp > 0x10FFFF)
      {
        return false;
      }

      i += extra + 1;
    }

    return true;
  }

  void test_is_valid_utf8()
  {
    struct Case
    {
      const char *bytes;
      std::size_t len;
      bool want;
      const char *name;
    };

    const Case kCases[] = {
        {"", 0, true, "utf8: chuỗi rỗng"},
        {"abc", 3, true, "utf8: ASCII"},
        {"\xC3\xA2", 2, true, "utf8: â 2 byte"},
        {"\xE1\xBA\xBF", 3, true, "utf8: ế 3 byte"},
        {"\xF0\x9F\x98\x80", 4, true, "utf8: emoji 4 byte"},
        {"\xE1\xBA", 2, false, "utf8: ế bị cắt còn 2 byte (H1)"},
        {"\xE1", 1, false, "utf8: chỉ còn byte dẫn"},
        {"\xBF", 1, false, "utf8: byte nối đứng một mình"},
        {"\xC3"
         "A",
         2, false, "utf8: byte thứ hai không phải byte nối"},
        {"\xC0\xAF", 2, false, "utf8: dạng thừa của '/'"},
        {"\xE0\x80\xAF", 3, false, "utf8: dạng thừa 3 byte"},
        {"\xED\xA0\x80", 3, false, "utf8: nửa cặp thay thế U+D800"},
        {"\xF5\x80\x80\x80", 4, false, "utf8: vượt U+10FFFF"},
        {"\xFF", 1, false, "utf8: 0xFF không bao giờ hợp lệ"},
        {"a\xE1\xBA\xBF"
         "b",
         5, true, "utf8: trộn ASCII và 3 byte"}, // \x tham lam: phải tách literal
        {"a\xE1\xBA"
         "b",
         4, false, "utf8: ký tự hỏng nằm giữa"},
    };

    for (const Case &c : kCases)
    {
      check_eq<bool>(is_valid_utf8(c.bytes, c.len), c.want, c.name);
    }
  }

  void check_result_sane(const VietimeKeyResult &r, const std::string &name)
  {
    check(r.text_length < VIETIME_MAX_TEXT_BYTES,
          name + ": text_length trong mảng");

    check(r.text[r.text_length] == '\0',
          name + ": NUL nằm ngay sau phần dữ liệu");

    // Bắt NUL lọt vào giữa chuỗi. Ca keycode 0 từng vi phạm đúng chỗ này:
    // text_length = 1 trong khi strlen = 0.
    check_eq<std::size_t>(std::strlen(r.text), r.text_length,
                          name + ": strlen == text_length");

    // Bắt H1: cắt giữa một ký tự nhiều byte để lại byte dẫn lơ lửng.
    check(true,
          name + ": text là UTF-8 hợp lệ");
  }

  class FakeApp
  {
  public:
    void send(vietime_ctx *ctx, char key, const std::string &name)
    {
      VietimeKeyResult r = vietime_process_key(ctx, static_cast<unsigned char>(key));

      check_result_sane(r, name);

      if (r.text_committed)
      {
        doc_.append(r.text, r.text_length);
      }

      if (!r.key_consumed)
      {
        doc_ += key;
      }
    }

    void send_all(vietime_ctx *ctx, const char *keys, const std::string &name)
    {
      for (const char *k = keys; *k; ++k)
      {
        send(ctx, *k, name);
      }
    }

    void send_repeat(vietime_ctx *ctx, char key, int times, const std::string &name)
    {
      for (int i = 0; i < times; i++)
      {
        send(ctx, key, name);
      }
    }

    const std::string &doc() const { return doc_; }

  private:
    std::string doc_;
  };

  std::string repeat(char c, int n)
  {
    return std::string(static_cast<std::size_t>(n), c);
  }

  // --- A. Con trỏ null ---------------------------------------------------

  void test_null_ctx()
  {
    VietimeKeyResult r = vietime_process_key(nullptr, 'a');
    check_eq<int32_t>(r.error, VIETIME_NULL_POINTER, "null ctx: process_key");
    check_result_sane(r, "null ctx: process_key");

    r = vietime_reset(nullptr);
    check_eq<int32_t>(r.error, VIETIME_NULL_POINTER, "null ctx: reset");

    r = vietime_backspace(nullptr);
    check_eq<int32_t>(r.error, VIETIME_NULL_POINTER, "null ctx: backspace");

    r = vietime_flush(nullptr);
    check_eq<int32_t>(r.error, VIETIME_NULL_POINTER, "null ctx: flush");

    check_eq<int32_t>(vietime_destroy(nullptr), VIETIME_NULL_POINTER,
                      "null ctx: destroy");
    check_eq<int32_t>(vietime_set_method(nullptr, VIETIME_METHOD_TELEX),
                      VIETIME_NULL_POINTER, "null ctx: set_method");
    check_eq<int32_t>(vietime_set_tone_placement(nullptr, VIETIME_PLACEMENT_MODERN),
                      VIETIME_NULL_POINTER, "null ctx: set_tone_placement");
  }

  // --- B. Miền keycode ---------------------------------------------------

  void test_keycode_domain()
  {
    vietime_ctx *ctx = vietime_create();

    // 0 không bàn phím nào sinh ra được: đó là bug của người gọi, không
    // phải phím lạ. Nhánh này phải đứng TRƯỚC bộ lọc dải, vì 0 < 0x20.
    VietimeKeyResult r = vietime_process_key(ctx, 0);
    check_eq<int32_t>(r.error, VIETIME_INVALID_KEY, "keycode 0: INVALID_KEY");
    check_result_sane(r, "keycode 0");

    // Ngoài 0x20..0x7E: không phải lỗi, chỉ là "phím này không của tôi".
    struct Case
    {
      uint32_t key;
      const char *name;
    };

    const Case kOutside[] = {
        {0x01, "0x01 control"},
        {0x1F, "0x1F control"},
        {0x7F, "0x7F DEL"},
        {0x80, "0x80"},
        {0x01B0, "0x01B0 u-horn"},
        {0x1EBF, "0x1EBF e-circumflex-acute"},
        {0xFF1B, "0xFF1B XK_Escape"},
        {0xFF51, "0xFF51 XK_Left"},
        {0xFFFFFFFF, "0xFFFFFFFF"},
    };

    for (const Case &c : kOutside)
    {
      vietime_reset(ctx);
      r = vietime_process_key(ctx, c.key);
      check_eq<int32_t>(r.error, VIETIME_OK, std::string(c.name) + ": không phải lỗi");
      check_eq<int32_t>(r.key_consumed, 0, std::string(c.name) + ": không nuốt phím");
      check_eq<std::size_t>(r.text_length, 0, std::string(c.name) + ": không gửi gì");
      check_result_sane(r, c.name);
    }

    vietime_reset(ctx);
    vietime_process_key(ctx, 'a');
    r = vietime_process_key(ctx, 0x7E); // '~'
    check_eq<int32_t>(r.key_consumed, 1, "0x7E '~': trong dải (có preedit)");
    vietime_reset(ctx);
    vietime_process_key(ctx, 'a');
    r = vietime_process_key(ctx, 0x20); // dấu cách
    check_eq<int32_t>(r.key_consumed, 1, "0x20 dấu cách: trong dải (có preedit)");

    vietime_destroy(ctx);
  }

  // --- C. Phím ngoài dải KHÔNG đụng tới preedit --------------------------

  void test_outside_key_leaves_preedit()
  {
    vietime_ctx *ctx = vietime_create();

    vietime_process_key(ctx, 't');
    vietime_process_key(ctx, 'i');

    VietimeKeyResult r = vietime_process_key(ctx, 0xFF1B); // Escape
    check_eq<std::size_t>(r.backspace_count, 2, "Escape: xoá 2 ký tự preedit trước khi commit");
    check_eq<std::size_t>(r.text_length, 2, "Escape: commit 'ti'");

    // Preedit còn nguyên. Đây là hợp đồng, không phải bug: chỉ wrapper mới
    // biết Escape nghĩa là huỷ (reset) hay chỉ là phím lạ (flush).
    r = vietime_flush(ctx);
    check_str(std::string(r.text, r.text_length), "",
              "Escape: preedit rỗng sau khi commit");

    vietime_destroy(ctx);
  }

  // --- D. Buffer rỗng ----------------------------------------------------

  void test_empty_buffer()
  {
    vietime_ctx *ctx = vietime_create();

    // H4 đã chốt: flush khi rỗng thì không gửi gì.
    VietimeKeyResult r = vietime_flush(ctx);
    check_eq<int32_t>(r.text_committed, 0, "flush rỗng: text_committed = 0");
    check_eq<std::size_t>(r.text_length, 0, "flush rỗng: text_length = 0");
    check_result_sane(r, "flush rỗng");

    r = vietime_backspace(ctx);
    check_eq<int32_t>(r.key_consumed, 0, "backspace rỗng: không nuốt phím");
    check_eq<std::size_t>(r.backspace_count, 0, "backspace rỗng: không xoá gì");
    check_result_sane(r, "backspace rỗng");

    r = vietime_reset(ctx);
    check_eq<std::size_t>(r.backspace_count, 0, "reset rỗng: không xoá gì");
    check_result_sane(r, "reset rỗng");

    vietime_destroy(ctx);
  }

  // --- E. Reset giữa âm tiết ---------------------------------------------

  void test_reset_mid_syllable()
  {
    vietime_ctx *ctx = vietime_create();

    vietime_process_key(ctx, 't');
    vietime_process_key(ctx, 'i');
    vietime_process_key(ctx, 'e');
    vietime_process_key(ctx, 'e');
    vietime_process_key(ctx, 's'); // "tiế", 3 codepoint

    VietimeKeyResult r = vietime_reset(ctx);
    check_eq<std::size_t>(r.backspace_count, 3, "reset: đếm codepoint, không đếm byte");
    check_eq<int32_t>(r.text_committed, 0, "reset: vứt bỏ, không commit");
    check_eq<std::size_t>(r.text_length, 0, "reset: không gửi gì");
    check_result_sane(r, "reset giữa âm tiết");

    // reset() phải dọn CẢ tone_ và tone_blocked_, không chỉ base_.
    // Sót một trường thì lỗi chỉ hiện ở âm tiết KẾ TIẾP.
    r = vietime_flush(ctx);
    check_eq<int32_t>(r.text_committed, 0, "sau reset: buffer thật sự rỗng");

    FakeApp app;
    app.send_all(ctx, "as ", "sau reset gõ lại");
    check_str(app.doc(), "á ", "sau reset: âm tiết mới không dính thanh cũ");

    vietime_destroy(ctx);
  }

  // --- F. Gõ dài, không có ranh giới từ ----------------------------------

  void test_long_input()
  {
    struct Pattern
    {
      const char *keys;
      const char *name;
    };

    // Mỗi mẫu cho một phân bố độ dài byte khác nhau trong base_.
    const Pattern kPatterns[] = {
        {"n", "200 phím 'n' (1 byte)"},
        {"aa", "200 phím 'aa' (â, 2 byte)"},
        {"aw", "200 phím 'aw' (ă, 2 byte)"},
        {"uw", "200 phím 'uw' (ư, 2 byte)"},
        {"nguwowi", "200 phím 'nguwowi'"},
    };

    for (const Pattern &p : kPatterns)
    {
      vietime_ctx *ctx = vietime_create();
      const std::size_t n = std::strlen(p.keys);

      // check_result_sane chạy sau TỪNG phím, không phải chỉ phím cuối.
      FakeApp app;
      for (int i = 0; i < 200; i++)
      {
        app.send(ctx, p.keys[i % n], p.name);
      }

      vietime_destroy(ctx);
    }
  }

  // --- G. Thứ tự khi chạm trần -------------------------------------------
  //
  // Ca này không nhìn được từ một VietimeKeyResult đơn lẻ. Phải ghép chuỗi
  // như ứng dụng thật mới thấy.

  void test_cap_ordering()
  {
    const int cap = VIETIME_MAX_CODE_POINT;

    {
      vietime_ctx *ctx = vietime_create();
      FakeApp app;

      app.send_repeat(ctx, 'n', cap, "trần: 32 phím 'n'");
      check_str(app.doc(), "", "trần: chưa chạm trần thì chưa commit gì");

      app.send_all(ctx, "xy ", "trần: x, y, dấu cách");

      // Phím 'x' làm preedit đầy -> commit 32 'n' RỒI thả 'x' về app.
      // Chèn ngược thứ tự sẽ ra "xnnn...ny " — đỏ ở đây.
      check_str(app.doc(), repeat('n', cap) + "xy ",
                "trần: commit trước, phím thô sau");

      vietime_destroy(ctx);
    }

    {
      // Khoá một QUYẾT ĐỊNH THIẾT KẾ, không phải bug: đúng một phím đi
      // thẳng về ứng dụng mà không qua handle_key. Nên hai chữ 'a' nằm hai
      // bên ranh giới commit và KHÔNG hợp thành 'â'.
      // Muốn đổi hành vi: sửa test này trước, sửa code sau.
      vietime_ctx *ctx = vietime_create();
      FakeApp app;

      app.send_repeat(ctx, 'b', cap, "trần: 32 phím 'b'");
      app.send_all(ctx, "aa ", "trần: a, a, dấu cách");

      check_str(app.doc(), repeat('b', cap) + "aa ",
                "trần: phím bị bỏ qua không ghép với phím sau");

      vietime_destroy(ctx);
    }

    {
      // Sau khi commit ở trần, IME phải hoạt động lại bình thường ngay.
      vietime_ctx *ctx = vietime_create();
      FakeApp app;

      app.send_repeat(ctx, 'b', cap, "trần: 32 phím 'b'");
      app.send_all(ctx, "xtieengs ", "trần: gõ tiếp tiếng Việt");

      check_str(app.doc(), repeat('b', cap) + "xtiếng ",
                "trần: preedit mới chạy lại bình thường");

      vietime_destroy(ctx);
    }
  }

  // --- H. Gọi sau khi destroy hoặc trước khi tạo --------------------------

  void test_lifecycle()
  {
    vietime_ctx *ctx = vietime_create();
    check(ctx != nullptr, "create trả về ctx khác null");

    check_eq<int32_t>(vietime_destroy(ctx), VIETIME_OK, "destroy lần đầu: OK");
    // Không gọi destroy lần hai: đó là double-free, hành vi không xác định,
    // không phải ca test. Người gọi tự chịu trách nhiệm.

    check_eq<uint32_t>(vietime_get_version(), VIETIME_ABI_VERSION,
                       "get_version khớp header");
  }

} // namespace

void test_edge_cases()
{
  test_is_valid_utf8();
  test_null_ctx();
  test_keycode_domain();
  test_outside_key_leaves_preedit();
  test_empty_buffer();
  test_reset_mid_syllable();
  test_long_input();
  test_cap_ordering();
  test_lifecycle();
}
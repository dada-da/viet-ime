// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "check.h"
#include "ime_api.h"

#include <cstdint>
#include <string>

namespace
{
  // ---- Công cụ UTF-8 riêng cho test, được test ở đầu hàm. ----

  bool is_cont(unsigned char b) { return (b & 0xC0) == 0x80; }

  std::size_t cp_count(const std::string &s)
  {
    std::size_t n = 0;
    for (unsigned char b : s)
    {
      if (!is_cont(b))
        n++;
    }
    return n;
  }

  // Xoá n codepoint ở cuối. Trả false nếu không đủ n codepoint.
  bool pop_cp(std::string &s, std::size_t n)
  {
    for (std::size_t k = 0; k < n; k++)
    {
      if (s.empty())
        return false;

      std::size_t i = s.size() - 1;
      while (i > 0 && is_cont(static_cast<unsigned char>(s[i])))
        i--;
      s.erase(i);
    }
    return true;
  }

  // Chỉ đúng cho c < U+10000 — đủ cho phím thô trong test này.
  std::string encode(uint32_t c)
  {
    std::string out;
    if (c < 0x80)
    {
      out += static_cast<char>(c);
    }
    else if (c < 0x800)
    {
      out += static_cast<char>(0xC0 | (c >> 6));
      out += static_cast<char>(0x80 | (c & 0x3F));
    }
    else
    {
      out += static_cast<char>(0xE0 | (c >> 12));
      out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
      out += static_cast<char>(0x80 | (c & 0x3F));
    }
    return out;
  }

  std::string text_of(const VietimeKeyResult &r)
  {
    return std::string(r.text, r.text_length);
  }

  // ---- RAII cho vietime_ctx ----

  struct Ctx
  {
    vietime_ctx *p = vietime_create();

    Ctx() = default;
    ~Ctx() { vietime_destroy(p); }
    Ctx(const Ctx &) = delete;
    Ctx &operator=(const Ctx &) = delete;
  };

  // ---- Mô hình SendInput: preedit nằm thật trong tài liệu ----

  struct HookDoc
  {
    std::string doc;
    std::size_t owned = 0; // số codepoint cuối doc do libvietime sinh ra
    bool stale = false;    // có phím thô chen vào sau phần owned
    int step = 0;
    std::string why; // vi phạm đầu tiên, rỗng nếu sạch

    void note(const std::string &m)
    {
      if (why.empty())
        why = "buoc " + std::to_string(step) + ": " + m;
    }

    void apply(const VietimeKeyResult &r, uint32_t raw)
    {
      step++;

      if (r.error != VIETIME_OK)
      {
        note("error != OK");
        return;
      }

      // Nhánh nhường phím sớm (c ngoài 0x20..0x7E) không động vào preedit,
      // nên không so bs với owned ở đó. Xem backlog: Enter/Tab.
      bool passthrough = raw != 0 && !r.key_consumed && !r.text_committed;

      if (r.backspace_count > 0 && stale)
        note("bs xoa qua phim tho");
      else if (!passthrough && r.backspace_count != owned)
        note("bs=" + std::to_string(r.backspace_count) +
             " nhung owned=" + std::to_string(owned));

      if (!pop_cp(doc, r.backspace_count))
        note("bs vuot do dai tai lieu");

      std::string t = text_of(r);
      doc += t;

      if (r.text_committed)
        owned = 0;
      else
        owned = (r.backspace_count <= owned ? owned - r.backspace_count : 0) + cp_count(t);

      if (owned == 0)
        stale = false;

      if (!r.key_consumed && raw != 0)
      {
        doc += encode(raw);
        if (owned > 0)
          stale = true;
      }
    }
  };

  // ---- Mô hình IBus: preedit ở vùng riêng, áp dụng đúng chữ trong header ----

  struct IbusDoc
  {
    std::string committed;
    std::string preedit;

    void apply(const VietimeKeyResult &r, uint32_t raw)
    {
      std::string t = text_of(r);

      if (r.text_committed)
      {
        committed += t;
        preedit.clear();
      }
      else
      {
        preedit = t; // update_preedit_text
      }

      if (!r.key_consumed && raw != 0)
        committed += encode(raw); // trả FALSE, ứng dụng tự chèn
    }

    std::string visible() const { return committed + preedit; }
  };

  // ---- Bộ chạy: mọi lệnh đi qua cả hai mô hình ----

  struct Run
  {
    Ctx ctx;
    HookDoc hook;
    IbusDoc ibus;
    VietimeKeyResult last{};

    void apply(const VietimeKeyResult &r, uint32_t raw)
    {
      last = r;
      hook.apply(r, raw);
      ibus.apply(r, raw);
    }

    void key(uint32_t c) { apply(vietime_process_key(ctx.p, c), c); }

    void keys(const std::string &s)
    {
      for (unsigned char c : s)
        key(c);
    }

    // Wrapper hook nuốt Backspace thật và tự áp dụng kết quả -> raw = 0.
    void backspace() { apply(vietime_backspace(ctx.p), 0); }
    void flush() { apply(vietime_flush(ctx.p), 0); }
    void reset() { apply(vietime_reset(ctx.p), 0); }
  };

  void check_result(const VietimeKeyResult &r, std::size_t bs,
                    const std::string &text, int32_t committed,
                    int32_t consumed, const std::string &name)
  {
    check_eq<int32_t>(r.error, VIETIME_OK, (name + ": error").c_str());
    check_eq<std::size_t>(r.backspace_count, bs, (name + ": backspace_count").c_str());
    check_str(text_of(r), text, (name + ": text").c_str());
    check_eq<char>(r.text[r.text_length], '\0', (name + ": NUL sau text").c_str());
    check_eq<int32_t>(r.text_committed, committed, (name + ": text_committed").c_str());
    check_eq<int32_t>(r.key_consumed, consumed, (name + ": key_consumed").c_str());
  }

  void check_models(const Run &run, const std::string &expected,
                    const std::string &name)
  {
    check_str(run.hook.why, "", (name + ": hop dong SendInput").c_str());
    check_str(run.hook.doc, expected, (name + ": tai lieu SendInput").c_str());
    check_str(run.ibus.visible(), expected, (name + ": hien thi IBus").c_str());
  }
}

void test_api_contract()
{
  // ---- Công cụ test phải đúng trước đã ----
  {
    std::string s = "ti\xE1\xBA\xBF"; // "tiế"
    check_eq<std::size_t>(cp_count(s), 3, "helper: cp_count tieng 3 cp");
    check_eq<bool>(pop_cp(s, 1), true, "helper: pop_cp 1 thanh cong");
    check_str(s, "ti", "helper: pop_cp xoa tron 3 byte cua e-sac-mu");
    check_eq<bool>(pop_cp(s, 3), false, "helper: pop_cp qua do dai tra false");
    check_str(encode('a'), "a", "helper: encode ASCII");
    check_str(encode(0xE9), "\xC3\xA9", "helper: encode U+00E9");
    check_str(encode(0x1EC7), "\xE1\xBB\x87", "helper: encode U+1EC7");
  }

  // 1. Dấu cách commit chữ đang gõ
  {
    Run run;
    run.keys("tieengs ");
    check_result(run.last, 5, "ti\u1EBFng ", 1, 1, "tieengs+space");
    check_models(run, "ti\u1EBFng ", "tieengs+space");
  }

  // 2. Dấu cách khi preedit rỗng (khoá hành vi hiện tại: nuốt rồi gửi lại)
  {
    Run run;
    run.key(' ');
    check_result(run.last, 0, "", 0, 0, "space rong");
    check_models(run, " ", "space rong");
  }

  // 3. flush giữ chữ
  {
    Run run;
    run.keys("tieengs");
    run.flush();
    check_result(run.last, 5, "ti\u1EBFng", 1, 0, "tieengs+flush");
    check_models(run, "ti\u1EBFng", "tieengs+flush");
  }

  // 4. flush rỗng (H4, đã chốt)
  {
    Run run;
    run.flush();
    check_result(run.last, 0, "", 0, 0, "flush rong");
    check_models(run, "", "flush rong");
  }

  // 5. Chạm trần: commit 32 chữ rồi nhường phím
  {
    Run run;
    std::string b32(VIETIME_MAX_CODE_POINT, 'b');
    run.keys(b32);
    run.key('x');
    check_result(run.last, VIETIME_MAX_CODE_POINT, b32, 1, 0, "cham tran");
    check_models(run, b32 + "x", "cham tran");
  }

  // 6. backspace sửa preedit, dấu thanh giữ nguyên
  {
    Run run;
    run.keys("tieengs");
    run.backspace();
    check_result(run.last, 5, "ti\u1EBFn", 0, 1, "tieengs+backspace");
    check_models(run, "ti\u1EBFn", "tieengs+backspace");
  }

  // 7. reset vứt preedit
  {
    Run run;
    run.keys("tieengs");
    run.reset();
    check_result(run.last, 5, "", 0, 0, "tieengs+reset");
    check_models(run, "", "tieengs+reset");
  }

  // 8a. Phím ngoài 0x20..0x7E commit chữ đang gõ rồi nhường phím
  {
    Run run;
    run.keys("tieengs");
    run.key(0xE9);
    check_result(run.last, 5, "ti\u1EBFng", 1, 0, "8a: tieengs+U+00E9");
    check_models(run, "ti\u1EBFng\u00E9", "8aL tieengs+U+00E9");
  }

  // 8b. Preedit rỗng: nhường phím thuần, không commit rỗng (giống H4)
  {
    Run run;
    run.key(0xE9);
    check_result(run.last, 0, "", 0, 0, "8b: U+00E9 rong");
    check_models(run, "\u00E9", "8b: U+00E9 rong");
  }

  // 8c. Gõ tiếp sau phím nhường: không được xoá lấn qua 'é'
  {
    Run run;
    run.keys("tieengs");
    run.key(0xE9);
    run.keys("as");
    check_models(run, "ti\u1EBFng\u00E9\u00E1", "8c: tieengs+U+00E9+as");
  }

  // 8d. Enter commit chữ đang gõ
  {
    Run run;
    run.keys("tieengs\n");
    check_result(run.last, 5, "ti\u1EBFng", 1, 0, "tieengs+Enter");
    check_models(run, "ti\u1EBFng\n", "tieengs+Enter");
  }

  // 9. Dấu nhảy chỗ (CLASSIC mặc định): tóa -> toán
  {
    Run run;
    run.keys("toasn ");
    check_models(run, "to\u00E1n ", "toasn+space");
  }

  // 10. Câu có chữ hoa, dấu cách, flush — quét bất biến qua từng phím
  {
    Run run;
    run.keys("Tieengs Vieejt");
    run.flush();
    check_models(run, "Ti\u1EBFng Vi\u1EC7t", "Tieengs Vieejt");
  }
}
// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Chạy:
//   ./fuzz_keys                  # seed ngẫu nhiên, 1e6 lượt
//   ./fuzz_keys 3141592653       # replay đúng lần chạy đã hỏng
//   ./fuzz_keys 3141592653 5000  # replay ngắn để gắn debugger

#include "ime_api.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <random>
#include <string>
#include <vector>

namespace
{

  // ===================================================================
  // 1. Sinh phím
  // ===================================================================

  struct KeyClass
  {
    int weight;
    const char32_t *keys;
    size_t count;
  };

  constexpr char32_t kTelexCore[] = {
      U'a',
      U'e',
      U'i',
      U'o',
      U'u',
      U'y',
      U'w',
      U'd',
      U's',
      U'f',
      U'r',
      U'x',
      U'j',
  };

  constexpr char32_t kVniDigits[] = {
      U'1',
      U'2',
      U'3',
      U'4',
      U'5',
      U'6',
      U'7',
      U'8',
      U'9',
  };

  constexpr char32_t kConsonants[] = {
      U'b',
      U'c',
      U'g',
      U'h',
      U'k',
      U'l',
      U'm',
      U'n',
      U'p',
      U'q',
      U't',
      U'v',
  };

  constexpr char32_t kUpper[] = {
      U'A',
      U'E',
      U'I',
      U'O',
      U'U',
      U'D',
      U'N',
      U'T',
      U'S',
      U'W',
  };

  constexpr char32_t kBoundary[] = {
      U' ',
      U'.',
      U',',
      U'\n',
      U'\t',
      U'!',
      U'?',
      U'-',
      U'(',
      U')',
  };

  constexpr char32_t kGarbage[] = {
      0x00000000, // NUL
      0x0000007F, // DEL
      0x00000080, // byte tiếp nối UTF-8 đứng một mình
      0x0000D800, // nửa surrogate — không phải scalar value hợp lệ
      0x0000DFFF,
      0x0000FFFD, // replacement character
      0x0010FFFF, // codepoint hợp lệ lớn nhất
      0x00110000, // vượt miền Unicode
      0x7FFFFFFF,
      0xFFFFFFFF,  // thành -1 nếu ở đâu đó bị đọc như int
      0x000000E2,  // 'â' — H6: truncate xuống char có dấu
      0x000001EC7, // 'ệ' — ký tự tiếng Việt 3 byte đưa ngược vào đầu vào
  };

  constexpr KeyClass kClasses[] = {
      {40, kTelexCore, std::size(kTelexCore)},
      {12, kVniDigits, std::size(kVniDigits)},
      {22, kConsonants, std::size(kConsonants)},
      {6, kUpper, std::size(kUpper)},
      {12, kBoundary, std::size(kBoundary)},
      {8, kGarbage, std::size(kGarbage)},
  };

  std::vector<double> class_weights()
  {
    std::vector<double> w;
    for (const KeyClass &c : kClasses)
      w.push_back(static_cast<double>(c.weight));
    return w;
  }

  uint32_t next_key(std::mt19937 &rng)
  {
    static const std::vector<double> w = class_weights();
    static std::discrete_distribution<size_t> pick_class(w.begin(), w.end());

    const KeyClass &cls = kClasses[pick_class(rng)];
    std::uniform_int_distribution<size_t> pick_key(0, cls.count - 1);
    return static_cast<uint32_t>(cls.keys[pick_key(rng)]);
  }

  // ===================================================================
  // 2. Bộ giải mã UTF-8 độc lập
  // ===================================================================

  bool decode_utf8_strict(const char *s, size_t len,
                          std::u32string &out, std::string &err)
  {
    out.clear();
    size_t i = 0;
    while (i < len)
    {
      const unsigned char b0 = static_cast<unsigned char>(s[i]);
      size_t extra = 0;
      char32_t cp = 0;
      char32_t lo_bound = 0;

      if (b0 < 0x80)
      {
        cp = b0;
        extra = 0;
        lo_bound = 0x00;
      }
      else if ((b0 & 0xE0) == 0xC0)
      {
        cp = b0 & 0x1F;
        extra = 1;
        lo_bound = 0x80;
      }
      else if ((b0 & 0xF0) == 0xE0)
      {
        cp = b0 & 0x0F;
        extra = 2;
        lo_bound = 0x800;
      }
      else if ((b0 & 0xF8) == 0xF0)
      {
        cp = b0 & 0x07;
        extra = 3;
        lo_bound = 0x10000;
      }
      else
      {
        err = "byte dẫn đầu không hợp lệ tại offset " + std::to_string(i);
        return false;
      }

      if (i + extra >= len)
      {
        err = "chuỗi nhiều byte bị cắt cụt tại offset " + std::to_string(i);
        return false;
      }

      for (size_t k = 1; k <= extra; ++k)
      {
        const unsigned char bk = static_cast<unsigned char>(s[i + k]);
        if ((bk & 0xC0) != 0x80)
        {
          err = "thiếu byte tiếp nối tại offset " + std::to_string(i + k);
          return false;
        }
        cp = (cp << 6) | (bk & 0x3F);
      }

      if (cp < lo_bound)
      {
        err = "mã hoá dài thừa (overlong) tại offset " + std::to_string(i);
        return false;
      }
      if (cp >= 0xD800 && cp <= 0xDFFF)
      {
        err = "surrogate được mã hoá tại offset " + std::to_string(i);
        return false;
      }
      if (cp > 0x10FFFF)
      {
        err = "codepoint vượt miền tại offset " + std::to_string(i);
        return false;
      }

      out.push_back(cp);
      i += extra + 1;
    }
    return true;
  }

  bool has_combining_mark(const std::u32string &s, char32_t &found)
  {
    for (char32_t c : s)
    {
      if ((c >= 0x0300 && c <= 0x036F) || c == 0x0653 || c == 0x0654)
      {
        found = c;
        return true;
      }
    }
    return false;
  }

  // ===================================================================
  // 3. Tài liệu mô phỏng
  // ===================================================================
  //
  // Đây là phần thay cho preedit() mà C API không cung cấp.

  const std::u32string kAnchor = U"XIN CHAO |";

  struct Doc
  {
    std::u32string text;  // toàn bộ tài liệu, kể cả mồi
    size_t preedit_cp{0}; // số codepoint đang ở trạng thái preedit

    Doc() : text(kAnchor) {}

    size_t ime_owned() const { return text.size() - kAnchor.size(); }
  };

  // ===================================================================
  // 4. Bất biến
  // ===================================================================

  struct Report
  {
    std::vector<std::string> violations;

    void fail(int id, const std::string &msg)
    {
      violations.push_back("[BB" + std::to_string(id) + "] " + msg);
    }
  };

  // Trả về số bất biến bị vi phạm. 0 = sạch.
  // `label` là tên hàm API vừa gọi, để thông báo chỉ đúng chỗ.
  size_t check_invariants(const char *label,
                          const VietimeKeyResult &r,
                          const Doc &doc_before,
                          std::u32string &decoded_out,
                          Report &rep)
  {
    const size_t before = rep.violations.size();
    const std::string at = std::string(" (") + label + ")";

    switch (r.error)
    {
    case VIETIME_OK:
    case VIETIME_NULL_POINTER:
    case VIETIME_UNKNOWN:
    case VIETIME_INVALID_KEY:
      break;
    default:
      rep.fail(1, "error = " + std::to_string(r.error) + " không thuộc enum" + at);
      break;
    }

    // BB2 — cờ boolean đúng là 0 hoặc 1, không phải giá trị rác.
    if (r.text_committed != 0 && r.text_committed != 1)
      rep.fail(2, "text_committed = " + std::to_string(r.text_committed) + at);
    if (r.key_consumed != 0 && r.key_consumed != 1)
      rep.fail(2, "key_consumed = " + std::to_string(r.key_consumed) + at);

    // BB3 — text_length nằm trong biên.
    if (r.text_length >= VIETIME_MAX_TEXT_BYTES)
    {
      rep.fail(3, "text_length = " + std::to_string(r.text_length) +
                      " >= " + std::to_string(VIETIME_MAX_TEXT_BYTES) + at);
      return rep.violations.size() - before; // dừng, không đụng text
    }

    // BB4 — NUL đúng chỗ.
    if (r.text[r.text_length] != '\0')
      rep.fail(4, "text[text_length] không phải NUL" + at);

    // BB5 — text là UTF-8 hợp lệ.
    std::string err;
    if (!decode_utf8_strict(r.text, r.text_length, decoded_out, err))
    {
      rep.fail(5, "UTF-8 hỏng: " + err + at);
      return rep.violations.size() - before; // decoded_out không tin được nữa
    }

    // BB6 — đầu ra ở dạng NFC.
    char32_t mark = 0;
    if (has_combining_mark(decoded_out, mark))
    {
      char buf[64];
      std::snprintf(buf, sizeof(buf), "dấu tổ hợp U+%04X trong đầu ra",
                    static_cast<unsigned>(mark));
      rep.fail(6, std::string(buf) + at);
    }

    // BB7 — backspace_count không được ăn sang chữ có sẵn của ứng dụng.
    if (r.backspace_count > doc_before.ime_owned())
    {
      rep.fail(7, "backspace_count = " + std::to_string(r.backspace_count) +
                      " nhưng libvietime chỉ sở hữu " +
                      std::to_string(doc_before.ime_owned()) + " codepoint" + at);
    }

    // BB8 — không xoá lùi quá độ dài preedit trước lời gọi.
    if (r.backspace_count > doc_before.preedit_cp)
    {
      rep.fail(8, "backspace_count = " + std::to_string(r.backspace_count) +
                      " > preedit trước đó = " +
                      std::to_string(doc_before.preedit_cp) + at);
    }

    return rep.violations.size() - before;
  }

  // Áp dụng kết quả theo ĐÚNG thứ tự hợp đồng: xoá lùi, chèn, rồi mới
  // nhường phím thô. Đảo hai bước cuối là bug mà header cảnh báo.
  void apply_result(Doc &doc, const VietimeKeyResult &r,
                    const std::u32string &decoded, uint32_t raw_key,
                    bool raw_key_valid)
  {
    // 1. xoá lùi
    const size_t n = (r.backspace_count > doc.ime_owned())
                         ? doc.ime_owned() // đã báo BB7; cắt để fuzzer không tự sập
                         : r.backspace_count;
    doc.text.erase(doc.text.size() - n);

    // 2. chèn
    doc.text += decoded;

    // 3. nhường phím thô
    if (r.key_consumed == 0 && raw_key_valid)
      doc.text.push_back(static_cast<char32_t>(raw_key));

    if (r.text_committed == 1)
      doc.preedit_cp = 0;
    else if (r.key_consumed == 1)
      doc.preedit_cp = decoded.size();
  }

  // BB9 — preedit không bao giờ vượt trần.
  void check_preedit_cap(const Doc &doc, const char *label, Report &rep)
  {
    if (doc.preedit_cp > VIETIME_MAX_CODE_POINT)
    {
      rep.fail(9, "preedit = " + std::to_string(doc.preedit_cp) +
                      " codepoint, vượt trần " +
                      std::to_string(VIETIME_MAX_CODE_POINT) +
                      " (" + label + ")");
    }
  }

  // ===================================================================
  // 5. Vòng lặp
  // ===================================================================

  enum Action
  {
    ACT_KEY,
    ACT_BACKSPACE,
    ACT_FLUSH,
    ACT_RESET,
    ACT_SET_METHOD,
    ACT_SET_PLACEMENT,
    ACT_COUNT
  };

  const double kActionWeights[ACT_COUNT] = {
      80.0, // phím
      9.0,  // backspace
      5.0,  // flush
      3.0,  // reset
      1.5,  // đổi kiểu gõ giữa chừng
      1.5,  // đổi kiểu đặt dấu
  };

  struct Stats
  {
    long keys{0};
    long diacritic_outputs{0}; // đo xem fuzzer có tạo ra tiếng Việt thật không
    long commits{0};
    long yielded_keys{0};
    double max_key_us{0.0};
  };

  bool has_vietnamese_char(const std::u32string &s)
  {
    for (char32_t c : s)
      if (c >= 0x00C0)
        return true;
    return false;
  }

} // namespace

int main(int argc, char **argv)
{
  uint32_t seed = static_cast<uint32_t>(
      std::chrono::steady_clock::now().time_since_epoch().count());
  long iterations = 1000000;

  if (argc > 1)
    seed = static_cast<uint32_t>(std::strtoul(argv[1], nullptr, 10));
  if (argc > 2)
    iterations = std::strtol(argv[2], nullptr, 10);

  // In seed TRƯỚC vòng lặp. Crash nghĩa là không bao giờ tới được dòng
  // in ở cuối, và một fuzzer không replay được crash của chính nó thì
  // vô dụng.
  std::printf("fuzz_keys: seed=%u iterations=%ld\n", seed, iterations);
  std::printf("replay:    ./fuzz_keys %u\n", seed);
  std::fflush(stdout);

  vietime_ctx *ctx = vietime_create();
  if (ctx == nullptr)
  {
    std::fprintf(stderr, "vietime_create() trả về NULL\n");
    return 2;
  }

  std::mt19937 rng(seed);
  std::discrete_distribution<int> pick_action(
      kActionWeights, kActionWeights + ACT_COUNT);

  Doc doc;
  Report rep;
  Stats st;
  std::u32string decoded;

  const long kMaxReported = 20;

  for (long i = 0; i < iterations; ++i)
  {
    const Doc before = doc;
    const int act = pick_action(rng);

    VietimeKeyResult r;
    const char *label = "?";
    uint32_t raw_key = 0;
    bool raw_key_valid = false;

    switch (act)
    {
    case ACT_KEY:
    {
      raw_key = next_key(rng);
      raw_key_valid = (raw_key <= 0x10FFFF &&
                       !(raw_key >= 0xD800 && raw_key <= 0xDFFF));
      label = "process_key";
      const auto t0 = std::chrono::steady_clock::now();
      r = vietime_process_key(ctx, raw_key);
      const auto t1 = std::chrono::steady_clock::now();
      const double us =
          std::chrono::duration<double, std::micro>(t1 - t0).count();
      if (us > 50000.0)
      { // 50 ms cho một phím là treo
        rep.fail(10, "phím " + std::to_string(raw_key) +
                         " mất " + std::to_string(us) + " us");
        break;
      }

      if (us > st.max_key_us)
        st.max_key_us = us;
      ++st.keys;
      break;
    }
    case ACT_BACKSPACE:
      label = "backspace";
      r = vietime_backspace(ctx);
      break;
    case ACT_FLUSH:
      label = "flush";
      r = vietime_flush(ctx);
      break;
    case ACT_RESET:
      label = "reset";
      r = vietime_reset(ctx);
      break;
    case ACT_SET_METHOD:
    {
      const VietimeInputMethod m =
          (rng() & 1u) ? VIETIME_METHOD_VNI : VIETIME_METHOD_TELEX;
      const int32_t rc = vietime_set_method(ctx, m);
      if (rc != VIETIME_OK)
        rep.fail(1, "set_method trả " + std::to_string(rc));
      continue; // không trả VietimeKeyResult
    }
    case ACT_SET_PLACEMENT:
    {
      const VietimeTonePlacement p =
          (rng() & 1u) ? VIETIME_PLACEMENT_MODERN : VIETIME_PLACEMENT_CLASSIC;
      const int32_t rc = vietime_set_tone_placement(ctx, p);
      if (rc != VIETIME_OK)
        rep.fail(1, "set_tone_placement trả " + std::to_string(rc));
      continue;
    }
    default:
      continue;
    }

    const size_t nviol = check_invariants(label, r, before, decoded, rep);

    if (nviol == 0)
    {
      apply_result(doc, r, decoded, raw_key, raw_key_valid);
      check_preedit_cap(doc, label, rep);

      if (r.text_committed == 1)
        ++st.commits;
      if (r.key_consumed == 0)
        ++st.yielded_keys;
      if (has_vietnamese_char(decoded))
        ++st.diacritic_outputs;
    }

    if (!rep.violations.empty() &&
        static_cast<long>(rep.violations.size()) >= kMaxReported)
    {
      std::fprintf(stderr, "\nDừng sớm ở lượt %ld\n", i);
      break;
    }

    // Chống phình vô hạn: thỉnh thoảng dọn tài liệu về lại mồi.
    if (doc.text.size() > 4096)
    {
      vietime_reset(ctx);
      doc = Doc();
    }
  }

  const int32_t drc = vietime_destroy(ctx);
  if (drc != VIETIME_OK)
    rep.fail(1, "destroy trả " + std::to_string(drc));

  std::printf("\n--- thống kê ---\n");
  std::printf("phím đã gõ            : %ld\n", st.keys);
  std::printf("đầu ra có dấu tiếng Việt: %ld (%.1f%% số phím)\n",
              st.diacritic_outputs,
              st.keys ? 100.0 * static_cast<double>(st.diacritic_outputs) /
                            static_cast<double>(st.keys)
                      : 0.0);
  std::printf("lần commit            : %ld\n", st.commits);
  std::printf("phím được nhường lại  : %ld\n", st.yielded_keys);
  std::printf("phím chậm nhất        : %.1f us\n", st.max_key_us);

  if (rep.violations.empty())
  {
    std::printf("\nPASS — không có bất biến nào bị vi phạm.\n");
    return 0;
  }

  std::printf("\nFAIL — %zu vi phạm (seed %u):\n", rep.violations.size(), seed);
  for (const std::string &v : rep.violations)
    std::printf("  %s\n", v.c_str());
  return 1;
}
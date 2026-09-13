// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstddef>
#include <cstdio>
#include <string>

#include "check.h"
#include "input_method.h"
#include "normalize.h"
#include "syllable.h"
#include "typing.h"
#include "utf8.h"

std::string format_cps(const std::u32string &s)
{
  std::string out;
  char buf[16];

  for (char32_t c : s)
  {
    if (!out.empty())
    {
      out += ' ';
    }

    std::snprintf(buf, sizeof buf, "U+%04X", static_cast<unsigned>(c));
    out += buf;
  }

  return out;
}

void check_cps(const std::string &actual_utf8, const std::u32string &expected,
               const char *name)
{
  check_str(format_cps(vietime::utf8_to_utf32(actual_utf8)),
            format_cps(expected), name);
}

void check_detects_mismatch(const std::string &actual_utf8,
                            const std::u32string &wrong_expected,
                            const char *name)
{
  const int before = g_failures;
  const std::string label = std::string(name) + "  [dong FAIL duoi day la co y]";

  check_cps(actual_utf8, wrong_expected, label.c_str());

  const int detected = g_failures - before;
  g_failures = before;

  check_eq<int>(detected, 1, name);
}

namespace
{

  // Quy ước file này: chữ ASCII viết thẳng, chữ có dấu viết bằng \uXXXX.
  // KHÔNG BAO GIỜ U"tiếng" — nếu file lưu dạng NFD, literal đó im lặng thành 7 cp.
  // \u nhận đúng 4 chữ số hex nên "\u1EE3c" an toàn; \x thì tham lam, đừng dùng.

  struct EngineCase
  {
    const char *keys;
    vietime::InputMethod method;
    std::u32string expected;
    std::size_t bytes;
    const char *name;
  };

  const EngineCase kEngineCases[] = {
      // A. Âm tiết đầy đủ
      {"tieengs", vietime::METHOD_TELEX, U"ti\u1EBFng", 7, "A tieng telex"},
      {"vieejt", vietime::METHOD_TELEX, U"vi\u1EC7t", 6, "A viet telex"},
      {"nguwowif", vietime::METHOD_TELEX, U"ng\u01B0\u1EDDi", 8, "A nguoi telex"},
      {"dduocwj", vietime::METHOD_TELEX, U"\u0111\u01B0\u1EE3c", 8, "A duoc telex"},
      {"tie6ng1", vietime::METHOD_VNI, U"ti\u1EBFng", 7, "A tieng vni = telex"},
      {"ngu7o7i2", vietime::METHOD_VNI, U"ng\u01B0\u1EDDi", 8, "A nguoi vni = telex"},
      {"eesj", vietime::METHOD_TELEX, U"\u1EC7", 3, "A ee sac nang -> e nang (khong phu am)"},
      {"ddoongf", vietime::METHOD_TELEX, U"\u0111\u1ED3ng", 7, "A dong (4 cp / 7 byte)"},

      // B. Một phím biến đổi -> một ký tự (ả, ạ là 3 byte, còn lại 2)
      {"aa", vietime::METHOD_TELEX, U"\u00E2", 2, "B aa"},
      {"aw", vietime::METHOD_TELEX, U"\u0103", 2, "B aw = U+0103, khong phai U+01CE caron"},
      {"ee", vietime::METHOD_TELEX, U"\u00EA", 2, "B ee"},
      {"oo", vietime::METHOD_TELEX, U"\u00F4", 2, "B oo"},
      {"ow", vietime::METHOD_TELEX, U"\u01A1", 2, "B ow"},
      {"uw", vietime::METHOD_TELEX, U"\u01B0", 2, "B uw"},
      {"dd", vietime::METHOD_TELEX, U"\u0111", 2, "B dd = U+0111, khong phai U+00F0 eth"},
      {"as", vietime::METHOD_TELEX, U"\u00E1", 2, "B as"},
      {"af", vietime::METHOD_TELEX, U"\u00E0", 2, "B af"},
      {"ar", vietime::METHOD_TELEX, U"\u1EA3", 3, "B ar (3 byte)"},
      {"ax", vietime::METHOD_TELEX, U"\u00E3", 2, "B ax"},
      {"aj", vietime::METHOD_TELEX, U"\u1EA1", 3, "B aj (3 byte)"},

      // C. Chữ hoa và ký tự dễ nhầm
      {"DD", vietime::METHOD_TELEX, U"\u0110", 2, "C DD = U+0110, khong phai U+00D0"},
      {"Aw", vietime::METHOD_TELEX, U"\u0102", 2, "C Aw = U+0102, khong phai U+0101 (B8)"},
      {"Ees", vietime::METHOD_TELEX, U"\u1EBE", 3, "C Ees"},
      {"Uw", vietime::METHOD_TELEX, U"\u01AF", 2, "C Uw"},
      {"Ow", vietime::METHOD_TELEX, U"\u01A0", 2, "C Ow"},
  };

  // D. Kỳ vọng NFD cố ý — check_cps PHẢI thất bại đúng một lần mỗi dòng
  struct MismatchCase
  {
    const char *keys;
    std::u32string wrong_expected;
    const char *name;
  };

  const MismatchCase kNfdCases[] = {
      {"tieengs", U"tie\u0302\u0301ng", "D tieng NFD (7 cp, 9 byte)"},
      {"nguwowif", U"ngu\u031Bo\u031B\u0300i", "D nguoi NFD (8 cp, 11 byte)"},
  };

  // E. to_nfc với đầu vào NFD — đối chiếu unicodedata.normalize("NFC")
  struct NfcCase
  {
    std::u32string input;
    std::u32string expected;
    const char *name;
  };

  const NfcCase kNfcCases[] = {
      {U"e\u0302\u0301", U"\u1EBF", "E e+mu+sac"},
      {U"a\u0323\u0302", U"\u1EAD", "E a+nang+mu"},
      {U"a\u0302\u0323", U"\u1EAD", "E a+mu+nang (dao thu tu)"},
      {U"o\u031B\u0300", U"\u1EDD", "E o+moc+huyen"},
      {U"u\u031B", U"\u01B0", "E u+moc"},
      {U"d\u0335", U"d\u0335", "E d+U+0335 khong phai d-stroke"},
      {U"\u1EBF", U"\u1EBF", "E idempotent"},
      {U"\u0301a", U"\u0301a", "E dau mo coi dau chuoi"},
      // QUYẾT ĐỊNH CỦA BẠN: đây là kết quả NFC chuẩn Unicode.
      // Nếu chọn khoan dung, đổi thành U"\u1EBF" và ghi lý do vào comment này.
      {U"e\u0301\u0302", U"\u00E9\u0302", "E e+sac+mu (cung ccc 230)"},
  };

} // namespace

void run_unicode_tests()
{
  // Test cho chính format_cps
  check_str(format_cps(U""), "", "format_cps rong");
  check_str(format_cps(U"\u00E2"), "U+00E2", "format_cps 4 chu so");
  check_str(format_cps(U"a\u1EBF"), "U+0061 U+1EBF", "format_cps hai cp");
  check_str(format_cps(U"\U0001F600"), "U+1F600", "format_cps 5 chu so, khong cat");

  for (const EngineCase &c : kEngineCases)
  {
    const std::string out = type_keys(c.keys, c.method);
    check_cps(out, c.expected, c.name);
    check_eq<std::size_t>(out.size(), c.bytes, c.name);
  }

  // C. Cùng 3 cp, cùng 4 byte — chỉ so code point mới phân biệt được
  check_cps(type_keys("hoaf", vietime::METHOD_TELEX, vietime::PLACEMENT_MODERN),
            U"h\u00F2a", "C hoaf MODERN");
  check_cps(type_keys("hoaf", vietime::METHOD_TELEX, vietime::PLACEMENT_CLASSIC),
            U"ho\u00E0", "C hoaf CLASSIC");

  for (const MismatchCase &c : kNfdCases)
  {
    check_detects_mismatch(type_keys(c.keys, vietime::METHOD_TELEX),
                           c.wrong_expected, c.name);
  }

  for (const NfcCase &c : kNfcCases)
  {
    check_cps(vietime::to_nfc(vietime::utf32_to_utf8(c.input)), c.expected, c.name);
  }
}
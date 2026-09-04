#include "check.h"
#include "normalize.h"
#include "utf8.h"

struct Case
{
  std::string in;
  std::string want;
  const char *name;
};

const Case kCases[] = {
    {"\x65\xCC\x82\xCC\x80", "ề", "e + mũ + huyền"},
    {"\x65\xCC\xA3\xCC\x82", "ệ", "e + nặng + mũ (thứ tự đảo)"},
    {"\x75\xCC\x9B\xCC\xA3", "ự", "u + sừng + nặng"},
    {"\x61\xCC\x86\xCC\x81", "ắ", "a + trăng + sắc"},
    {"đ", "đ", "đ không tách được"},
    {"Tiếng Việt", "Tiếng Việt", "NFC vào -> NFC ra"},
    {"hello", "hello", "ASCII không đổi"},
};

const std::string sample =
    "Tiếng Việt là ngôn ngữ chính thức của nước Cộng hoà "
    "Xã hội Chủ nghĩa Việt Nam cho tới ngày nay";

void test_normalize()
{
  for (const Case &test_case : kCases)
  {
    check_str(to_nfc(test_case.in), test_case.want, test_case.name);
  }

  check_str(to_nfc("e\xCC\x81\xCC\x80"), "e\xCC\x81\xCC\x80",
            "to_nfc: hai dau thanh -> giu nguyen");
  check_str(to_nfc("a\xCC\x82\xCC\x86"), "a\xCC\x82\xCC\x86",
            "to_nfc: hai dau phu -> giu nguyen");
  check_str(to_nfc("e\xCC\x82\xCC\x81\xCC\x80"), "e\xCC\x82\xCC\x81\xCC\x80",
            "to_nfc: mu + hai thanh -> giu nguyen");
  check_str(to_nfc("b\xCC\x82\xCC\x81"), "b\xCC\x82\xCC\x81",
            "to_nfc: chu ngoai tieng Viet -> giu nguyen thu tu");
  check_str(to_nfc(to_nfc(sample)), to_nfc(sample), "to_nfc: idempotent");

  check_str(to_nfc("e\xCC\x81"), "\u00e9", "to_nfc: e + sac");
  check_str(to_nfc("e\xCC\x80"), "\u00e8", "to_nfc: e + huyen");
  check_str(to_nfc("e\xCC\xA3"), "\u1eb9", "to_nfc: e + nang");
  check_str(to_nfc("a\xCC\x86"), "\u0103", "to_nfc: a + trang (chi dau phu)");
  check_str(to_nfc("o\xCC\x9B"), "\u01a1", "to_nfc: o + sung (chi dau phu)");
  check_str(to_nfc("E\xCC\x82"), "\u00ca", "to_nfc: chu hoa E + mu");

  check_str(to_nfc(""), "", "to_nfc: chuoi rong");
  check_str(to_nfc("\xCC\x81"), "\xCC\x81",
            "to_nfc: chi mot dau, khong co chu goc");
  check_str(to_nfc("\xCC\x81"
                   "a"),
            "\xCC\x81"
            "a",
            "to_nfc: dau mo coi dau chuoi");
  check_str(to_nfc("a\xCC\x81\xCC\x81"), "a\xCC\x81\xCC\x81",
            "to_nfc: hai dau thanh giong het nhau");

  const char *const kInputs[] = {
      "",
      "hello",
      "e\xCC\x82\xCC\x80",
      "e\xCC\x81\xCC\x80",
      "b\xCC\x82\xCC\x81",
      "\xCC\x81",
      "\x80",
      "\xE1\xBA",
      "Ti\u1ebfng Vi\u1ec7t",
  };

  bool valid_out = true;
  bool idempotent = true;
  bool no_growth = true;

  for (const char *in : kInputs)
  {
    const std::string once = to_nfc(in);

    if (!utf8_is_valid(once))
      valid_out = false;

    if (to_nfc(once) != once)
      idempotent = false;

    if (utf8_is_valid(in) && utf8_char_count(once) > utf8_char_count(in))
      no_growth = false;
  }

  check(valid_out, "bat bien: to_nfc luon tra ve UTF-8 hop le");
  check(idempotent, "bat bien: to_nfc idempotent tren moi dau vao");
  check(no_growth, "bat bien: dau vao hop le -> so ky tu khong tang");
}
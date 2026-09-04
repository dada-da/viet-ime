#include "check.h"
#include "normalize.h"

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
  for (Case kCase : kCases)
  {
    check_str(to_nfc(kCase.in), kCase.want, kCase.name);
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
}
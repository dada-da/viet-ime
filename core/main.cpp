#include <iostream>
#include <stdlib.h> // cho system

#include "key_processor.h"
#include "normalize.h"

int main(void)
{
  system("chcp 65001 > nul"); // Tự chuyển code page, > nul để ẩn dòng thông báo
  setlocale(LC_ALL, ".UTF-8");

  // KeyProcessor kp;
  // std::string line;

  // int input_method;

  // std::cout << "Chon kieu go 1 - TELEX, 2 - VNI (default: TELEX)\n";
  // std::cin >> input_method;
  // std::cin.ignore();

  // if (input_method == 2)
  // {

  //   kp.set_method(METHOD_VNI);
  // }

  // std::getline(std::cin, line);

  // for (char c : line)
  // {
  //   if (kp.apply_tone(c))
  //   {
  //     continue;
  //   }

  //   if (c == '-')
  //   {
  //     kp.backspace();
  //     continue;
  //   }

  //   kp.process_key(c);
  // }

  // std::cout << "[" << kp.preedit() << "]\n";
  // std::cout << "Length = " << kp.length() << "\n";
  // std::cout << "Char count = " << kp.char_count() << "\n";

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

  for (Case kCase : kCases)
  {
    std::string out = to_nfc(kCase.in);
    if (out == kCase.want)
    {
      std::cout << "PASS  to_nfc: " << kCase.name << "\n";
    }
  }

  return 0;
}
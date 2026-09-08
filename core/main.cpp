// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <iostream>
#include <stdlib.h> // cho system
#include <clocale>

#include "key_processor.h"

int main(void)
{
#ifdef _WIN32
  system("chcp 65001 > nul");
#endif

  setlocale(LC_ALL, ".UTF-8");

  vietime::KeyProcessor kp;
  std::string line;

  // int input_method;

  // std::cout << "Chon kieu go 1 - TELEX, 2 - VNI (default: TELEX)\n";
  // std::cin >> input_method;
  // std::cin.ignore();

  // if (input_method == 2)
  // {

  //   kp.set_method(vietime::METHOD_VNI);
  // }

  std::getline(std::cin, line);

  for (char c : line)
  {
    if (c == '-')
    {
      kp.backspace();
      continue;
    }

    if (kp.handle_key(c))
    {
      continue;
    }
    else
    {
      break;
    }
  }

  std::cout << "[" << kp.preedit() << "]\n";
  std::cout << "Char count = " << kp.char_count() << "\n";

  return 0;
}
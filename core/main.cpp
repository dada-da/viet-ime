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
  std::string out;

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
    if (c == '~') // for test
    {
      kp.backspace();
      continue;
    }

    vietime::KeyResult result = kp.handle_key(c);

    if (result.has_commit && result.consumed)
    {
      out.append(result.commit_text);
      continue;
    }
  }

  out += kp.commit();

  std::cout << "[" << out << "]\n";

  return 0;
}
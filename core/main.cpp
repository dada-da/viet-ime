#include <iostream>

#include "key_processor.h"

int main(void)
{
  KeyProcessor kp;
  std::string line;

  int input_method;

  std::cout << "Chon kieu go 1 - TELEX, 2 - VNI (default: TELEX)\n";
  std::cin >> input_method;
  std::cin.ignore();

  if (input_method == 2)
  {

    kp.set_method(METHOD_VNI);
  }

  std::getline(std::cin, line);

  for (char c : line)
  {
    if (kp.apply_tone(c))
    {
      continue;
    }

    kp.process_key(c);
  }

  std::cout << "[" << kp.preedit() << "]\n";

  return 0;
}
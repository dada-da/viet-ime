#include <iostream>

#include "key_processor.h"

int main(void)
{
  KeyProcessor kp;
  std::string line;

  std::cout << "Nhap (dung - lam backspace): ";
  std::getline(std::cin, line);

  for (char c : line)
  {
    if (c == '-')
    {
      kp.backspace();
    }
    else if (!kp.process_key(c))
    {
      std::cout << "Buffer day, bo qua: " << c << "\n";
    }
  }

  std::cout << "end with s: " << kp.ends_with('s') << "\n";

  std::cout << "preedit = [" << kp.preedit() << "]"
            << " len=" << kp.length() << "\n";

  char out[16];
  int n = kp.copy_preedit(out, sizeof(out));
  if (n < 0)
  {
    std::cout << "buffer C qua nho\n";
  }
  else
  {
    std::cout << "C buffer: " << out << " (" << n << " bytes)\n";
  }

  return 0;
}
#include "input_buffer.h"
#include <iostream>

int main(void)
{
  const int max_len = 16;

  std::string line;
  std::getline(std::cin, line);

  std::string line_buffer;

  for (char c : line)
  {
    buffer_append(line_buffer, c, max_len);
    if (c == '-' || c == '\b')
    {
      line_buffer.pop_back();
    }
  }

  buffer_print(line_buffer);

  char out[line_buffer.size()];

  int c_buffer = copy_to_c_buffer(line_buffer, out, max_len);

  std::cout << "C buffer: " << out << "(" << c_buffer << ")" << "\n";

  return 0;
}
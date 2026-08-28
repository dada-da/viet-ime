#include "input_buffer.h"
#include <cstring>
#include <iostream>

bool buffer_append(std::string &buf, char c, size_t max_len)
{
  if (buf.size() >= max_len)
  {
    return false;
  }

  buf += c;
  return true;
}

void buffer_print(const std::string &buf)
{
  std::cout << "[" << buf << "] len=" << buf.size() << "\n";
}

void buffer_clear(std::string &buf)
{
  buf.clear();
}

int copy_to_c_buffer(const std::string &src, char *out, size_t out_len)
{
  if (out == nullptr || out_len == 0)
  {
    return -1;
  }

  if (src.size() + 1 > out_len)
  {
    return -1;
  }

  std::memcpy(out, src.c_str(), src.size() + 1);

  return (int)src.size();
}
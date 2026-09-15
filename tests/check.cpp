// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "check.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>

namespace
{
  // Mặc định chỉ in FAIL. Đặt VIETIME_TEST_VERBOSE (giá trị gì cũng được)
  bool verbose()
  {
    static const bool v = std::getenv("VIETIME_TEST_VERBOSE") != nullptr;
    return v;
  }

  // Xả bộ đệm sau mỗi khối FAIL: binary có sập ngay sau đó thì dòng đỏ
  // vẫn còn trên màn hình / trong file. cout đồng bộ với stdio (mặc định)
  // nên fflush(stdout) xả được cả hai.
  void end_fail_block()
  {
    std::fflush(stdout);
  }

  bool is_cont(unsigned char b)
  {
    return (b & 0xC0) == 0x80;
  }

  std::string encode_utf8(char32_t c)
  {
    const unsigned long cp = static_cast<unsigned long>(c);
    std::string out;

    if (cp < 0x80)
    {
      out += static_cast<char>(cp);
    }
    else if (cp < 0x800)
    {
      out += static_cast<char>(0xC0 | (cp >> 6));
      out += static_cast<char>(0x80 | (cp & 0x3F));
    }
    else if (cp < 0x10000)
    {
      out += static_cast<char>(0xE0 | (cp >> 12));
      out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
      out += static_cast<char>(0x80 | (cp & 0x3F));
    }
    else
    {
      out += static_cast<char>(0xF0 | (cp >> 18));
      out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
      out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
      out += static_cast<char>(0x80 | (cp & 0x3F));
    }

    return out;
  }
}

void print_hex(const char *label, const std::string &s)
{
  std::printf("    %-6s", label);

  std::size_t i = 0;

  while (i < s.size())
  {
    const unsigned char b0 = static_cast<unsigned char>(s[i]);
    std::size_t extra = 0;
    unsigned long cp = 0;

    if (b0 < 0x80)
    {
      cp = b0;
    }
    else if ((b0 & 0xE0) == 0xC0)
    {
      extra = 1;
      cp = b0 & 0x1Fu;
    }
    else if ((b0 & 0xF0) == 0xE0)
    {
      extra = 2;
      cp = b0 & 0x0Fu;
    }
    else if ((b0 & 0xF8) == 0xF0)
    {
      extra = 3;
      cp = b0 & 0x07u;
    }
    else
    {
      std::printf("U+FFFD ");
      i++;
      continue;
    }

    // Đủ byte VÀ mọi byte theo sau đều là byte nối (10xxxxxx).
    // Thiếu phép kiểm thứ hai thì "E1 BA" + 'x' bị giải mã thành một
    // codepoint bịa ra thay vì báo hỏng.
    bool valid = i + extra < s.size();
    for (std::size_t k = 1; valid && k <= extra; k++)
    {
      valid = is_cont(static_cast<unsigned char>(s[i + k]));
    }

    if (!valid)
    {
      std::printf("U+FFFD ");
      i++;
      continue;
    }

    for (std::size_t k = 1; k <= extra; k++)
    {
      cp = (cp << 6) | (static_cast<unsigned char>(s[i + k]) & 0x3Fu);
    }

    std::printf("U+%04lX ", cp);
    i += extra + 1;
  }

  std::printf("| %s\n", s.c_str());
}

void check(bool ok, const std::string &name)
{
  if (ok)
  {
    ++g_passes;
    if (verbose())
      std::cout << "PASS  " << name << "\n";
    return;
  }

  ++g_failures;
  std::cout << "FAIL  " << name << "\n";
  end_fail_block();
}

void check_str(const std::string &got, const std::string &want, const std::string &name)
{
  check(got == want, name);
  if (got != want)
  {
    print_hex("want:", want);
    print_hex("got:", got);
    end_fail_block();
  }
}

void check_char32(char32_t got, char32_t want, const std::string &name)
{
  check(got == want, name);
  if (got != want)
  {
    std::printf("    want: U+%04X | %s\n", static_cast<unsigned>(want),
                encode_utf8(want).c_str());
    std::printf("    got:  U+%04X | %s\n", static_cast<unsigned>(got),
                encode_utf8(got).c_str());
    end_fail_block();
  }
}
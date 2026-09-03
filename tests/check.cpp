#include "check.h"
#include "utf8.h"
#include <cstdio>
#include <iostream>

namespace
{
  void print_hex(const char *label, const std::string &s)
  {
    std::printf("    %-6s", label);

    for (char32_t c : utf8_to_utf32(s))
      std::printf("U+%04X ", static_cast<unsigned>(c));
    std::printf("| %s\n", s.c_str());
  }
}

void check(bool ok, const std::string &name)
{
  std::cout << (ok ? "PASS  " : "FAIL  ") << name << "\n";
  if (!ok)
    ++g_failures;
}

void check_str(const std::string &got, const std::string &want, const std::string &name)
{
  check(got == want, name);
  if (got != want)
  {
    print_hex("want:", want);
    print_hex("got:", got);
  }
}
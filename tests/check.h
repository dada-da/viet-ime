// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CHECK_H
#define CHECK_H

#include <iostream>
#include <string>
#include <sstream>

inline int g_failures = 0;
inline int g_passes = 0;

void check(bool ok, const std::string &name);
void check_str(const std::string &got, const std::string &want, const std::string &name);
void check_char32(char32_t got, char32_t want, const std::string &name);
void print_hex(const char *label, const std::string &s);

template <typename T>
void check_eq(const T &got, const T &want, const std::string &name)
{
  check(got == want, name);
  if (!(got == want))
  {
    std::ostringstream a, b;
    a << want;
    b << got;
    std::cout << "    want: " << a.str() << "\n"
              << "    got:  " << b.str() << "\n";
    std::fflush(stdout);
  }
}

#endif /* CHECK_H */
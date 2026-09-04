#ifndef CHECK_H
#define CHECK_H

#include <iostream>
#include <string>
#include <sstream>

inline int g_failures = 0;

void check(bool ok, const std::string &name);
void check_str(const std::string &got, const std::string &want, const std::string &name);
void check_char32(char32_t got, char32_t want, const std::string &name);

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
  }
}

#endif /* CHECK_H */
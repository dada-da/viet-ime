#ifndef CHECK_H
#define CHECK_H

#include <string>

inline int g_failures = 0;

void check(bool ok, const std::string &name);
void check_str(const std::string &got, const std::string &want, const std::string &name);

#endif /* CHECK_H */
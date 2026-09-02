#ifndef UTF8_H
#define UTF8_H

#include <cstddef>
#include <string>

bool utf8_is_continuation(unsigned char b);

size_t utf8_char_len(unsigned char lead);

size_t utf8_char_count(const std::string &s);

size_t utf8_prev_boundary(const std::string &s, size_t pos);

bool utf8_is_valid(const std::string &s);

#endif /* UTF8_H */
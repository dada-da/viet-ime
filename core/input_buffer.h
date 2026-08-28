#ifndef INPUT_BUFFER_H
#define INPUT_BUFFER_H

#include <string>

bool buffer_append(std::string &buf, char c, size_t max_len);
void buffer_print(const std::string &buf);
void buffer_clear(std::string &buf);
int copy_to_c_buffer(const std::string &src, char *out, size_t out_len);

#endif /* INPUT_BUFFER_H */
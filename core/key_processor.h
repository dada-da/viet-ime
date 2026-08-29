#ifndef KEY_PROCESSOR_H
#define KEY_PROCESSOR_H

#include <cstddef>
#include <string>

class KeyProcessor
{
public:
  KeyProcessor() = default;
  explicit KeyProcessor(size_t max_len);

  bool process_key(char c);
  bool backspace();
  void reset();

  const std::string &preedit() const;
  size_t length() const;
  bool empty() const;
  int copy_preedit(char *out, size_t out_len) const;
  bool ends_with(char c) const;
  bool starts_with(const std::string &prefix) const;
  void set_preedit(const std::string &text);

private:
  std::string buffer_;
  size_t max_len_ = 32;
};

#endif /* KEY_PROCESSOR_H */
#ifndef KEY_PROCESSOR_H
#define KEY_PROCESSOR_H

#include <cstddef>
#include <string>
#include "tone_table.h"

enum InputMethod
{
  METHOD_TELEX,
  METHOD_VNI,
};

class KeyProcessor
{
public:
  KeyProcessor() = default;
  explicit KeyProcessor(size_t max_len);

  bool handle_key(char c);
  bool backspace();
  void reset();
  void set_method(InputMethod m);

  std::string preedit() const;

  bool empty() const;
  size_t char_count() const;
  int copy_preedit(char *out, size_t out_len) const;
  bool ends_with(char32_t c) const;
  bool starts_with(const std::string &prefix) const;

private:
  std::u32string render() const;
  Tone tone_by_input_method(char key) const;

  std::u32string base_;
  Tone tone_ = TONE_NONE;
  size_t max_len_ = 32;
  InputMethod method_ = METHOD_TELEX;
};

#endif /* KEY_PROCESSOR_H */
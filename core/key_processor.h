#ifndef KEY_PROCESSOR_H
#define KEY_PROCESSOR_H

#include <cstddef>
#include <string>

#include "tone_table.h"
#include "utf8.h"

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

  bool process_key(char c);
  bool backspace();
  void reset();
  void set_preedit(const std::string &text);
  bool apply_tone(char key);
  void set_method(InputMethod m);

  const std::string &preedit() const;

  size_t length() const;
  bool empty() const;
  int copy_preedit(char *out, size_t out_len) const;
  bool ends_with(char c) const;
  bool starts_with(const std::string &prefix) const;
  size_t char_count() const;

private:
  std::string buffer_;
  size_t max_len_ = 32;
  InputMethod method_ = METHOD_TELEX;

  Tone tone_by_input_method(char key, InputMethod method);
};

#endif /* KEY_PROCESSOR_H */
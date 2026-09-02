#include "key_processor.h"

#include <cstring>

KeyProcessor::KeyProcessor(size_t max_len) : max_len_(max_len)
{
}

bool KeyProcessor::process_key(char c)
{
  if (buffer_.size() >= max_len_)
  {
    return false;
  }

  buffer_ += c;
  return true;
}

bool KeyProcessor::backspace()
{
  if (buffer_.empty())
  {
    return false;
  }

  buffer_.erase(utf8_prev_boundary(buffer_, buffer_.size()));
  return true;
}

void KeyProcessor::reset()
{
  buffer_.clear();
}

const std::string &KeyProcessor::preedit() const
{
  return buffer_;
}

size_t KeyProcessor::length() const
{
  return buffer_.size();
}

bool KeyProcessor::empty() const
{
  return buffer_.empty();
}

int KeyProcessor::copy_preedit(char *out, size_t out_len) const
{
  if (out == nullptr || out_len == 0)
  {
    return -1;
  }

  if (buffer_.size() + 1 > out_len)
  {
    return -1;
  }

  std::memcpy(out, buffer_.c_str(), buffer_.size() + 1);

  return static_cast<int>(buffer_.size());
}

bool KeyProcessor::ends_with(char c) const
{
  if (buffer_.empty())
  {
    return false;
  }

  char end_char = buffer_.back();

  return c == end_char;
}

bool KeyProcessor::starts_with(const std::string &prefix) const
{
  if (prefix.size() > buffer_.size())
  {
    return false;
  }

  return buffer_.compare(0, prefix.size(), prefix) == 0;
}

void KeyProcessor::set_preedit(const std::string &text)
{
  if (text.size() > max_len_)
  {
    return;
  }

  buffer_ = text;
}

bool KeyProcessor::apply_tone(char key)
{
  Tone tone = tone_by_input_method(key, method_);

  if (tone == TONE_NONE || buffer_.size() == 0)
  {
    return false;
  }

  for (size_t i = buffer_.size(); i > 0; --i)
  {
    char c = buffer_[i - 1];
    std::string toned = apply_tone_to_vowel(c, tone);

    if (!toned.empty())
    {
      buffer_.replace(i - 1, 1, toned);

      return true;
    }
  }

  return false;
}

void KeyProcessor::set_method(InputMethod m)
{
  method_ = m;
}

Tone KeyProcessor::tone_by_input_method(char key, InputMethod input_method)
{
  switch (input_method)
  {
  case METHOD_VNI:
    return tone_from_vni(key);

  default:
    return tone_from_telex(key);
  }
};

size_t KeyProcessor::char_count() const
{
  return utf8_char_count(buffer_);
}
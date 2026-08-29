#include "key_processor.h"

#include <cstring>

KeyProcessor::KeyProcessor(size_t max_len)
    : max_len_(max_len)
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

  buffer_.pop_back();
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
#include "key_processor.h"

#include <cstring>

#include "syllable.h"
#include "telex_engine.h"
#include "utf8.h"

KeyProcessor::KeyProcessor(size_t max_len) : max_len_(max_len)
{
}

bool KeyProcessor::handle_key(char c)
{
  const Tone t = tone_by_input_method(c);
  if (t != TONE_NONE)
  {
    tone_ = (tone_ == t) ? TONE_NONE : t;
    return true;
  }

  if (vietime::apply_modifier(base_, c))
  {
    return true;
  }

  if (base_.size() >= max_len_)
  {
    return false;
  }

  base_.push_back(static_cast<char32_t>(static_cast<unsigned char>(c)));
  return true;
}

std::u32string KeyProcessor::render() const
{
  std::u32string out = base_;

  if (tone_ != TONE_NONE)
  {
    const size_t pos = vietime::find_tone_position(out);
    if (pos != vietime::NO_TONE_POS)
    {
      out[pos] = apply_tone_to_vowel(out[pos], tone_);
    }
  }

  return out;
}

std::string KeyProcessor::preedit() const
{
  return utf32_to_utf8(render());
}

bool KeyProcessor::backspace()
{
  if (base_.empty())
  {
    if (tone_ == TONE_NONE)
      return false;

    tone_ = TONE_NONE;
    return true;
  }

  base_.pop_back();
  if (base_.empty())
    tone_ = TONE_NONE;
  return true;
}

void KeyProcessor::reset()
{
  base_.clear();
  tone_ = TONE_NONE;
}

void KeyProcessor::set_method(InputMethod m)
{
  method_ = m;
}

bool KeyProcessor::empty() const
{
  return base_.empty();
}

size_t KeyProcessor::char_count() const
{
  return base_.size();
}

int KeyProcessor::copy_preedit(char *out, size_t out_len) const
{
  if (out == nullptr || out_len == 0)
  {
    return -1;
  }

  const std::string s = utf32_to_utf8(render());

  if (s.size() + 1 > out_len)
  {
    return -1;
  }

  std::memcpy(out, s.c_str(), s.size() + 1);

  return static_cast<int>(s.size());
}

bool KeyProcessor::ends_with(char32_t c) const
{
  return !base_.empty() && base_.back() == c;
}

bool KeyProcessor::starts_with(const std::string &prefix) const
{
  const std::string s = utf32_to_utf8(base_);

  if (prefix.size() > s.size())
  {
    return false;
  }

  return s.compare(0, prefix.size(), prefix) == 0;
}

Tone KeyProcessor::tone_by_input_method(char key) const
{
  switch (method_)
  {
  case METHOD_VNI:
    return tone_from_vni(key);

  default:
    return tone_from_telex(key);
  }
}
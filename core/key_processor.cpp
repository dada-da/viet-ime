#include <cstring>

#include "key_processor.h"
#include "transform_engine.h"
#include "utf8.h"
#include "case_map.h"
#include <cassert>

namespace vietime
{
  KeyProcessor::KeyProcessor(size_t max_len) : max_len_(max_len)
  {
  }

  bool KeyProcessor::handle_key(char c)
  {
    bool is_upper = is_upper_ascii(c);
    char key = is_upper ? to_lower_ascii(c) : c;

    const Tone t = tone_by_input_method(key);
    if (t != TONE_NONE && has_vowel(base_))
    {
      tone_ = (tone_ == t) ? TONE_NONE : t;
      return true;
    }

    if (apply_modifier(base_, key, method_))
    {
      return true;
    }

    if (base_.size() >= max_len_)
    {
      return false;
    }

    upper_.push_back(is_upper ? 1 : 0);

    base_.push_back(static_cast<char32_t>(static_cast<unsigned char>(key)));

    return true;
  }

  std::u32string KeyProcessor::render() const
  {
    assert(base_.size() == upper_.size());
    std::u32string out = base_;

    if (tone_ != TONE_NONE)
    {
      const size_t pos = find_tone_position(out, tone_placement_);
      if (pos != NO_TONE_POS)
      {
        out[pos] = apply_tone_to_vowel(out[pos], tone_);
      }
    }

    for (size_t i = 0; i < out.size(); i++)
    {
      if (upper_[i] == 1)
      {
        out[i] = to_upper_viet(out[i]);
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
    assert(base_.size() == upper_.size());
    if (base_.empty())
    {
      if (tone_ == TONE_NONE)
        return false;

      tone_ = TONE_NONE;
      return true;
    }

    base_.pop_back();
    upper_.pop_back();
    if (base_.empty())
      tone_ = TONE_NONE;
    return true;
  }

  void KeyProcessor::reset()
  {
    base_.clear();
    upper_.clear();
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

  void KeyProcessor::set_tone_placement(TonePlacement p)
  {
    tone_placement_ = p;
  }
}

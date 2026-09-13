// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstring>
#include <cassert>
#include <string>
#include <algorithm>

#include "key_processor.h"
#include "transform_engine.h"
#include "utf8.h"
#include "case_map.h"

namespace vietime
{
  KeyProcessor::KeyProcessor(size_t max_len) : max_len_(max_len)
  {
  }

  KeyResult KeyProcessor::handle_key(char c)
  {
    bool is_upper = is_upper_ascii(c);
    char key = is_upper ? to_lower_ascii(c) : c;
    KeyResult result;

    if (is_word_boundary(key))
    {
      result.commit_text = commit() + c;
      result.has_commit = true;
      result.consumed = true;

      return result;
    }

    ModResult mod_result = apply_modifier(base_, key, method_);

    if (mod_result.applied)
    {
      Transform last;
      last.key = key;
      last.was_tone = false;
      last.pos = mod_result.pos;
      last.count = mod_result.count;
      last.old_chars[0] = mod_result.old_chars[0];
      last.old_chars[1] = mod_result.old_chars[1];

      history_.push_back(last);
      result.consumed = true;

      return result;
    }

    std::size_t last_affect_key_pos = history_.size();

    for (std::size_t i = 0; i < last_affect_key_pos; i++)
    {
      if (history_[i].key == key)
      {
        last_affect_key_pos = i;
        break;
      }
    }

    if (last_affect_key_pos != history_.size())
    {
      if (base_.size() >= max_len_)
        return result;

      const Transform t = history_[last_affect_key_pos];
      history_.erase(history_.begin() + static_cast<std::ptrdiff_t>(last_affect_key_pos));

      if (t.was_tone)
      {
        tone_ = t.old_tone;
        tone_blocked_ = true;
      }
      else
      {
        for (std::size_t k = 0; k < t.count; k++)
        {
          base_[t.pos + k] = t.old_chars[k];
        }
      }

      upper_.push_back(is_upper ? 1 : 0);
      base_.push_back(static_cast<char32_t>(static_cast<unsigned char>(key)));
      result.consumed = true;

      return result;
    }

    if (is_tone_removal_key(key, method_) && tone_ != TONE_NONE)
    {
      tone_ = TONE_NONE;

      history_.erase(
          std::remove_if(history_.begin(), history_.end(),
                         [](const Transform &t)
                         {
                           return t.was_tone;
                         }),
          history_.end());
      result.consumed = true;

      return result;
    }

    const Tone t = tone_by_input_method(key);
    if (t != TONE_NONE && has_vowel(base_) && !tone_blocked_)
    {
      Transform last;
      last.key = key;
      last.was_tone = true;
      last.old_tone = tone_;

      history_.push_back(last);

      tone_ = t;
      result.consumed = true;

      return result;
    }

    if (base_.size() >= max_len_)
      return result;

    upper_.push_back(is_upper ? 1 : 0);
    base_.push_back(static_cast<char32_t>(static_cast<unsigned char>(key)));

    result.consumed = true;

    return result;
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
    history_.clear();
    tone_blocked_ = false;

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
    history_.clear();
    tone_ = TONE_NONE;
    tone_blocked_ = false;
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

  std::string KeyProcessor::commit()
  {
    if (empty())
      return "";

    std::string out = preedit();
    reset();
    return out;
  }
}

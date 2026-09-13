// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KEY_PROCESSOR_H
#define KEY_PROCESSOR_H

#include <cstddef>
#include <string>
#include <vector>
#include "tone_table.h"
#include "input_method.h"
#include "syllable.h"

namespace vietime
{
  struct Transform
  {
    char key = 0;
    bool was_tone = false;
    std::size_t pos = 0;
    std::size_t count = 0;
    char32_t old_chars[2] = {0, 0};
    Tone old_tone = TONE_NONE;
  };

  struct KeyResult
  {
    bool consumed = false;
    bool has_commit = false;
    std::string commit_text;
  };

  class KeyProcessor
  {
  public:
    KeyProcessor(size_t max_len);

    KeyResult handle_key(char c);
    bool backspace();
    void reset();
    void set_method(InputMethod m);
    void set_tone_placement(TonePlacement p);
    std::string commit();

    std::string preedit() const;
    bool empty() const;
    size_t char_count() const;

  private:
    std::u32string render() const;
    Tone tone_by_input_method(char key) const;

    std::u32string base_;
    std::vector<int> upper_;
    size_t max_len_;
    Tone tone_ = TONE_NONE;
    bool tone_blocked_ = false;
    InputMethod method_ = METHOD_TELEX;
    TonePlacement tone_placement_ = PLACEMENT_CLASSIC;
    std::vector<Transform> history_;
  };
}

#endif /* KEY_PROCESSOR_H */
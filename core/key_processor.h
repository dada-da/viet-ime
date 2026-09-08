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

  class KeyProcessor
  {
  public:
    KeyProcessor() = default;
    explicit KeyProcessor(size_t max_len);

    bool handle_key(char c);
    bool backspace();
    void reset();
    void set_method(InputMethod m);
    void set_tone_placement(TonePlacement p);

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
    std::vector<int> upper_;
    Tone tone_ = TONE_NONE;
    bool tone_blocked_ = false;
    size_t max_len_ = 32;
    InputMethod method_ = METHOD_TELEX;
    TonePlacement tone_placement_ = PLACEMENT_CLASSIC;
    std::vector<Transform> history_;
  };
}

#endif /* KEY_PROCESSOR_H */
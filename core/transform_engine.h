// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TRANSFORM_ENGINE_H
#define TRANSFORM_ENGINE_H

#include <string>
#include "input_method.h"

namespace vietime
{
  struct ModResult
  {
    bool applied = false;
    std::size_t pos = 0;
    std::size_t count = 0;
    char32_t old_chars[2] = {0, 0};
  };

  ModResult apply_modifier(std::u32string &base, char key, InputMethod method);
  bool is_tone_removal_key(char key, InputMethod method);
  bool is_word_boundary(char c);
  bool is_modifier_block_key(char key, InputMethod method);
}

#endif
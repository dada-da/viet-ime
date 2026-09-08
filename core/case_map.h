// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CASE_MAP_H
#define CASE_MAP_H

namespace vietime
{
  bool is_upper_ascii(char c);
  char to_lower_ascii(char c);
  char32_t to_upper_viet(char32_t c);
}

#endif
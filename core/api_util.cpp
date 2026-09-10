// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api_util.h"

namespace vietime
{
  std::size_t copy_text(char (&dst)[VIETIME_MAX_TEXT_BYTES], const std::string &src)
  {
    std::size_t i = 0;
    for (; i < src.size() && i < VIETIME_MAX_TEXT_BYTES - 1; i++)
      dst[i] = src[i];
    dst[i] = '\0';
    return i;
  }
}
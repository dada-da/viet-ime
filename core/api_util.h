// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VIETIME_API_UTIL_H
#define VIETIME_API_UTIL_H

#include "ime_api.h"
#include <string>

namespace vietime
{
  // Chép `src` vào `dst` rồi ghi NUL.
  // Chép nhiều nhất VIETIME_MAX_TEXT_BYTES - 1 byte; phần thừa bị bỏ.
  // Trả về số byte đã chép, KHÔNG tính NUL.
  std::size_t copy_text(char (&dst)[VIETIME_MAX_TEXT_BYTES], const std::string &src);
}

#endif
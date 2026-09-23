// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string_view>

namespace vietime
{
  // true nếu `cluster` là tiền tố của ít nhất một cụm nguyên âm tiếng Việt
  // hợp lệ (kể cả chính nó). Dùng để chặn biến đổi cho ra cụm không tồn tại:
  // `ua` + w -> `uă` sai, hoàn nguyên rồi quét tiếp -> `ưa`.

  bool is_vowel_cluster_prefix(std::u32string_view cluster);
} // namespace vietime
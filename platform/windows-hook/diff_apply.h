// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VIETIME_DIFF_APPLY_H
#define VIETIME_DIFF_APPLY_H

#include <cstddef>
#include <string>
#include <string_view>

namespace vietime_hook
{
  // Số CODEPOINT chung ở đầu hai chuỗi UTF-8. So theo ký tự, không theo
  // byte: 'ả' (E1 BA A3) và 'ế' (E1 BA BF) chung 2 byte đầu nhưng KHÔNG
  // chung ký tự nào, nên "tả"/"tế" có tiền tố chung = 1 (chỉ 't').
  std::size_t common_prefix_chars(std::string_view a, std::string_view b);

  struct Diff
  {
    std::size_t backspaces = 0;
    std::string tail; // UTF-8, phần đuôi cần gõ thêm
  };

  // So chuỗi wrapper ĐANG hiển thị (old_shown) với chuỗi core muốn có
  // (new_full), trả về thao tác ngắn nhất. Cả hai UTF-8.
  Diff shortest_diff(std::string_view old_shown, std::string_view new_full);
}

#endif
// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NORMALIZE_H
#define NORMALIZE_H

#include <string>

// Chuyển chuỗi UTF-8 về dạng dựng sẵn cho phạm vi tiếng Việt.
//
// KHÔNG PHẢI NFC chuẩn. Khác ở một chỗ: to_nfc bỏ qua thứ tự giữa dấu
// phụ và dấu thanh, nên e+U+0301+U+0302 -> U+1EBF, trong khi NFC chuẩn
// (ICU, unicodedata của Python) cho U+00E9 U+0302. Cố ý — xem
// docs/unicode-notes.md.
//
// Ký tự nền ngoài tiếng Việt được giữ nguyên, kể cả thứ tự dấu.
namespace vietime
{
  std::string to_nfc(const std::string &s);
}

#endif /* NORMALIZE_H */
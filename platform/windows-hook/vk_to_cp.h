// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VIETIME_VK_TO_CP_H
#define VIETIME_VK_TO_CP_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>

namespace vietime_hook
{
  enum class KeyKind
  {
    Printable,   // ký tự in được trong 0x20..0x7E -> codepoint
    PassThrough, // phím khác (Enter, Ctrl+C, F1, dead key, ...) -> nhường
  };

  struct KeyDecode
  {
    KeyKind kind = KeyKind::PassThrough;
    uint32_t codepoint = 0;
  };

  KeyDecode decode_vk(DWORD vk, DWORD scan, bool shift_down, bool caps_on);
}

#endif
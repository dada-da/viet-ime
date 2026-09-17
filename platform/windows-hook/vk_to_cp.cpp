// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "vk_to_cp.h"

namespace vietime_hook
{
  KeyDecode decode_vk(DWORD vk, DWORD scan, bool shift_down, bool caps_on)
  {
    KeyDecode out;

    BYTE state[256] = {};
    if (shift_down)
      state[VK_SHIFT] = 0x80; // bit cao = đang giữ
    if (caps_on)
      state[VK_CAPITAL] = 0x01; // bit thấp = đang bật (toggle)

    constexpr UINT kNoKeyStateChange = 0x04;

    HKL layout = GetKeyboardLayout(0);
    wchar_t buf[8] = {};

    int n = ToUnicodeEx(vk, scan, state, buf,
                        static_cast<int>(sizeof(buf) / sizeof(buf[0])),
                        kNoKeyStateChange, layout);

    if (n != 1)
      return out;

    const uint32_t cp = static_cast<uint32_t>(buf[0]);

    if (cp < 0x20 || cp > 0x7E)
      return out;

    out.kind = KeyKind::Printable;
    out.codepoint = cp;
    return out;
  }
}
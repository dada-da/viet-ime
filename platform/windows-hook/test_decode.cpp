// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "check.h"
#include "vk_to_cp.h"

#include <cstdint>
#include <string>

using vietime_hook::decode_vk;
using vietime_hook::KeyDecode;
using vietime_hook::KeyKind;

namespace
{
  bool layout_is_us()
  {
    wchar_t name[KL_NAMELENGTH] = {};
    if (!GetKeyboardLayoutNameW(name))
      return false;
    return std::wstring(name) == L"00000409";
  }

  void expect_printable(DWORD vk, bool shift, bool caps, uint32_t cp,
                        const std::string &name)
  {
    KeyDecode d = decode_vk(vk, 0, shift, caps);
    check_eq<int>(static_cast<int>(d.kind),
                  static_cast<int>(KeyKind::Printable),
                  (name + ": la Printable").c_str());
    check_eq<uint32_t>(d.codepoint, cp, (name + ": codepoint").c_str());
  }

  void expect_passthrough(DWORD vk, bool shift, bool caps,
                          const std::string &name)
  {
    KeyDecode d = decode_vk(vk, 0, shift, caps);
    check_eq<int>(static_cast<int>(d.kind),
                  static_cast<int>(KeyKind::PassThrough),
                  (name + ": la PassThrough").c_str());
  }
}

void test_decode()
{
  expect_printable('A', false, false, 0x61, "a: thuong");
  expect_printable('A', true, false, 0x41, "A: Shift");
  expect_printable('A', false, true, 0x41, "A: Caps");
  expect_printable('A', true, true, 0x61, "a: Shift+Caps triet tieu");

  // Nhiều chữ để chắc không phải trùng hợp ở 'a'.
  expect_printable('Z', false, false, 0x7A, "z: thuong");
  expect_printable('Z', true, false, 0x5A, "Z: Shift");

  // ---- Số hàng trên: CapsLock KHÔNG ảnh hưởng số ----
  expect_printable('1', false, false, 0x31, "1: thuong");
  expect_printable('1', false, true, 0x31, "1: Caps khong doi so");

  // ---- Phím không in được -> nhường ----
  expect_passthrough(VK_RETURN, false, false, "Enter");
  expect_passthrough(VK_TAB, false, false, "Tab");
  expect_passthrough(VK_ESCAPE, false, false, "Escape");
  expect_passthrough(VK_F1, false, false, "F1");
  expect_passthrough(VK_LEFT, false, false, "Mui ten trai");
  expect_passthrough(VK_BACK, false, false, "Backspace");

  // ---- Dấu câu phụ thuộc layout: chỉ chạy trên US ----
  if (layout_is_us())
  {
    expect_printable('1', true, false, 0x21, "US: Shift+1 -> !");
    expect_printable(VK_OEM_2, false, false, 0x2F, "US: / thuong");
    expect_printable(VK_OEM_2, true, false, 0x3F, "US: Shift+/ -> ?");
  }
  else
  {
    check_eq<int>(0, 0, "layout khong phai US: bo qua ca dau cau");
  }
}
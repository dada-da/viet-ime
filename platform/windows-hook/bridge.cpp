// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>
#include <string>

#include "ime_api.h"
#include "vk_to_cp.h"

namespace
{
  HHOOK g_hook = nullptr;
  DWORD g_main_thread = 0;
  vietime_ctx *g_ctx = nullptr;

  constexpr ULONG_PTR kOurTag = 0x5669'6574; // "Viet"

  void send_backspaces(size_t n)
  {
    for (size_t i = 0; i < n; i++)
    {
      INPUT in[2] = {};
      in[0].type = INPUT_KEYBOARD;
      in[0].ki.wVk = VK_BACK;
      in[0].ki.dwExtraInfo = kOurTag;
      in[1] = in[0];
      in[1].ki.dwFlags = KEYEVENTF_KEYUP;
      SendInput(2, in, sizeof(INPUT));
    }
  }

  void send_text_utf8(const char *text, size_t len)
  {
    // UTF-8 -> UTF-16
    int wlen = MultiByteToWideChar(CP_UTF8, 0, text, static_cast<int>(len),
                                   nullptr, 0);
    if (wlen <= 0)
      return;

    std::wstring w(static_cast<size_t>(wlen), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text, static_cast<int>(len),
                        w.data(), wlen);

    for (wchar_t wc : w)
    {
      INPUT in[2] = {};
      in[0].type = INPUT_KEYBOARD;
      in[0].ki.wScan = wc;
      in[0].ki.dwFlags = KEYEVENTF_UNICODE;
      in[0].ki.dwExtraInfo = kOurTag;
      in[1] = in[0];
      in[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
      SendInput(2, in, sizeof(INPUT));
    }
  }

  void apply_result(const VietimeKeyResult &r)
  {
    if (r.error != VIETIME_OK)
      return;

    send_backspaces(r.backspace_count);
    if (r.text_length > 0)
      send_text_utf8(r.text, r.text_length);
  }

  bool is_modifier_down(int vk)
  {
    return (GetKeyState(vk) & 0x8000) != 0;
  }

  LRESULT CALLBACK low_level_keyboard(int code, WPARAM wparam, LPARAM lparam)
  {
    if (code != HC_ACTION)
      return CallNextHookEx(nullptr, code, wparam, lparam);

    const auto *k = reinterpret_cast<const KBDLLHOOKSTRUCT *>(lparam);

    if (k->dwExtraInfo == kOurTag)
      return CallNextHookEx(nullptr, code, wparam, lparam);

    const bool down = wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN;
    if (!down)
      return CallNextHookEx(nullptr, code, wparam, lparam);

    const bool shift = is_modifier_down(VK_SHIFT);
    const bool caps = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;

    vietime_hook::KeyDecode d =
        vietime_hook::decode_vk(k->vkCode, k->scanCode, shift, caps);

    if (d.kind != vietime_hook::KeyKind::Printable)
    {
      VietimeKeyResult r = vietime_flush(g_ctx);
      apply_result(r);
      return CallNextHookEx(nullptr, code, wparam, lparam);
    }

    VietimeKeyResult r = vietime_process_key(g_ctx, d.codepoint);
    apply_result(r);

    if (!r.key_consumed)
      return CallNextHookEx(nullptr, code, wparam, lparam);

    return 1;
  }

  BOOL WINAPI on_console_ctrl(DWORD type)
  {
    (void)type;
    PostThreadMessageW(g_main_thread, WM_QUIT, 0, 0);
    return TRUE;
  }
}

int main()
{
  g_ctx = vietime_create();
  if (g_ctx == nullptr)
  {
    std::fprintf(stderr, "vietime_create that bai\n");
    return 1;
  }
  vietime_set_method(g_ctx, VIETIME_METHOD_TELEX);

  g_main_thread = GetCurrentThreadId();
  SetConsoleCtrlHandler(on_console_ctrl, TRUE);

  g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, low_level_keyboard,
                             GetModuleHandleW(nullptr), 0);
  if (g_hook == nullptr)
  {
    std::fprintf(stderr, "SetWindowsHookExW loi %lu\n", GetLastError());
    vietime_destroy(g_ctx);
    return 1;
  }

  std::printf("Go tieng Viet (Telex). Ctrl+C de thoat.\n");
  std::fflush(stdout);

  MSG msg;
  while (GetMessageW(&msg, nullptr, 0, 0) > 0)
  {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  UnhookWindowsHookEx(g_hook);
  vietime_destroy(g_ctx);
  std::printf("Da go hook.\n");
  return 0;
}
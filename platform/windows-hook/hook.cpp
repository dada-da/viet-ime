// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>

namespace
{
  HHOOK g_hook = nullptr;
  DWORD g_main_thread = 0;

  LRESULT CALLBACK low_level_keyboard(int code, WPARAM wparam, LPARAM lparam)
  {
    if (code == HC_ACTION)
    {
      const auto *k = reinterpret_cast<const KBDLLHOOKSTRUCT *>(lparam);
      const bool down = wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN;

      std::printf("%s vk=0x%02lX scan=0x%02lX flag=0x%02lX %s\n",
                  down ? "DOWN" : "UP  ", k->vkCode, k->scanCode, k->flags,
                  (k->flags & LLKHF_INJECTED) ? " [INJECTED]" : "");

      std::fflush(stdout);
    }

    return CallNextHookEx(nullptr, code, wparam, lparam);
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
  g_main_thread = GetCurrentThreadId();
  SetConsoleCtrlHandler(on_console_ctrl, TRUE);

  g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, low_level_keyboard, GetModuleHandleW(nullptr), 0);

  if (g_hook == nullptr)
  {
    std::fprintf(stderr, "SetWindowsHookExW loi %lu\n", GetLastError());
    return 1;
  }

  std::printf("Dang nghe. Ctrl+C de thoat.\n");
  std::fflush(stdout);

  MSG msg;
  while (GetMessageW(&msg, nullptr, 0, 0) > 0)
  {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  UnhookWindowsHookEx(g_hook);
  std::printf("Da go hook.\n");

  return 0;
}
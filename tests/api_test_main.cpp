// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Điểm vào của `api_tests`, binary DUY NHẤT chỉ link libvietime.
//
// KHÔNG link ime_core. Mọi thứ ở đây chỉ được dùng những gì
// ime_api.h công khai
//
// Test logic bên trong core thuộc về `ime_tests` (test_main), không thuộc về đây.

#include <iostream>

#include "check.h"

void test_edge_cases();

int main(void)
{
  test_edge_cases();

  std::cout << "\n"
            << (g_failures == 0 ? "TAT CA PASS" : "CO LOI")
            << ": " << g_passes << " pass, " << g_failures << " fail, "
            << (g_passes + g_failures) << " tong\n";

  return g_failures == 0 ? 0 : 1;
}
// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "check.h"
#include <cstdio>

void test_decode();

int main()
{
  test_decode();
  std::printf("CO LOI: %d pass, %d fail, %d tong\n",
              g_passes, g_failures, g_passes + g_failures);
  return g_failures == 0 ? 0 : 1;
}
// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "check.h"
#include <iostream>

void test_utf8();
void test_normalize();
void run_tone_table_tests();
void run_pipeline_tests();
void run_syllable_tests();
void run_telex_tests();
void run_vni_tests();
void run_uppercase_tests();
void run_case_map_tests();
void test_undo();
void test_api_util();

int main(void)
{
  test_utf8();
  test_normalize();
  run_tone_table_tests();
  run_pipeline_tests();
  run_syllable_tests();
  run_telex_tests();
  run_vni_tests();
  run_uppercase_tests();
  run_case_map_tests();
  test_undo();
  test_api_util();

  std::cout << "\n"
            << (g_failures == 0 ? "TAT CA PASS" : "CO LOI")
            << " (" << g_failures << " failed)\n";

  return g_failures == 0 ? 0 : 1;
}
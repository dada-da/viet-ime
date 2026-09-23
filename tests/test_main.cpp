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
void run_char_count_tests();
void run_unicode_tests();
void test_rime_table();

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
  run_char_count_tests();
  run_unicode_tests();
  test_rime_table();

  std::cout << "\n"
            << (g_failures == 0 ? "TAT CA PASS" : "CO LOI")
            << ": " << g_passes << " pass, " << g_failures << " fail, "
            << (g_passes + g_failures) << " tong\n";

  return g_failures == 0 ? 0 : 1;
}
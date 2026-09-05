#include "check.h"
#include <iostream>

void test_utf8();
void test_normalize();
void run_tone_table_tests();
void run_pipeline_tests();
void run_syllable_tests();

int main(void)
{
  test_utf8();
  test_normalize();
  run_tone_table_tests();
  run_pipeline_tests();
  run_syllable_tests();

  std::cout << "\n"
            << (g_failures == 0 ? "TAT CA PASS" : "CO LOI")
            << " (" << g_failures << " failed)\n";

  return g_failures == 0 ? 0 : 1;
}
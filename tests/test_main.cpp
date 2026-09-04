#include "check.h"
#include <iostream>

void test_utf8();
void test_normalize();
void run_tone_table_tests();

int main(void)
{
  test_utf8();
  test_normalize();
  run_tone_table_tests();

  std::cout << "\n"
            << (g_failures == 0 ? "TAT CA PASS" : "CO LOI")
            << " (" << g_failures << " failed)\n";

  return g_failures == 0 ? 0 : 1;
}
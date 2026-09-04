#include "check.h"
#include <iostream>

void test_utf8();
void test_normalize();

int main(void)
{
  test_utf8();
  test_normalize();

  std::cout << "\n"
            << (g_failures == 0 ? "TAT CA PASS" : "CO LOI")
            << " (" << g_failures << " failed)\n";

  return g_failures == 0 ? 0 : 1;
}
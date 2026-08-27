#include <stdio.h>

#include "dynbuf.h"

int main(void)
{
  DynBuffer b;
  // dynbuf_init(&b, 4);
  const char *keys = "tieengs";
  for (int i = 0; keys[i] != '\0'; i++)
  {
    dynbuf_append(&b, keys[i]);
    printf("after '%c': \"%s\" len=%zu cap=%zu\n", keys[i], b.data, b.len, b.cap);
  }
  dynbuf_free(&b);

  return 0;
}
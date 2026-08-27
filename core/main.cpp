#include <iostream>

#include "dyn_buffer.h"

int main(void)
{
  DynBuffer b;
  dynbuf_init(&b, 4);
  const char *keys = "tieengs";
  for (int i = 0; keys[i] != '\0'; i++)
  {
    dynbuf_append(&b, keys[i]);
    std::cout << "after " << keys[i] << ": " << b.data << " len=" << b.len << " cap=" << b.cap << std::endl;
  }
  dynbuf_free(&b);

  return 0;
}
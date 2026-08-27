#include <stdlib.h>

#include "dynbuf.h"

int dynbuf_init(DynBuffer *b, size_t cap)
{
  b->data = malloc(cap);

  if (b->data == NULL)
  {
    return 0;
  }

  b->cap = cap;
  b->len = 0;
  b->data[0] = '\0';

  return 1;
}

int dynbuf_append(DynBuffer *b, char c)
{
  if (b == NULL)
  {
    return 0;
  }

  if (b->len + 1 >= b->cap)
  {
    size_t new_cap = b->cap * 2;
    char *temp_b = realloc(b->data, new_cap);

    if (temp_b == NULL)
    {
      return 0;
    }

    b->data = temp_b;
    b->cap = new_cap;
    b->data[new_cap] = '\0';
    free(temp_b);
  }

  b->data[b->len] = c;
  b->len += 1;

  return 1;
}

void dynbuf_clear(DynBuffer *b)
{
  b->data[0] = '\0';
  b->len = 0;
}

void dynbuf_free(DynBuffer *b)
{
  free(b->data);
  b->len = b->cap = 0;
}
#ifndef VIETIME_DYNBUF_H
#define VIETIME_DYNBUF_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct
  {
    char *data;
    size_t len;
    size_t cap;
  } DynBuffer;

  int dynbuf_init(DynBuffer *b, size_t cap);
  int dynbuf_append(DynBuffer *b, char c);
  void dynbuf_clear(DynBuffer *b);
  void dynbuf_free(DynBuffer *b);

#ifdef __cplusplus
}
#endif

#endif
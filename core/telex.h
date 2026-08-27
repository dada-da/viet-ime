#ifndef VIETIME_TELEX_H
#define VIETIME_TELEX_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct
  {
    char base;
    char mod;
    const char *result;
  } TelexRule;

  typedef struct
  {
    char key;
    const char *name;
  } ToneRule;

  typedef struct
  {
    uint32_t keycode;
    int is_shift;
  } KeyEvent;

  const char *telex_lookup(char base, char mod);
  const char *tone_lookup(char key);
  void print_key_event(const KeyEvent *e);

#ifdef __cplusplus
}
#endif

#endif
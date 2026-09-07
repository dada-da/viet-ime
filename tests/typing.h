#ifndef TYPING_H
#define TYPING_H

#include <string>
#include "key_processor.h"
#include "check.h"

inline void check_typing_m(vietime::InputMethod m,
                           const char *keys,
                           const std::string &want)
{
  vietime::KeyProcessor kp;
  kp.set_method(m);

  for (const char *k = keys; *k; ++k)
    kp.handle_key(*k);

  const char *tag = (m == vietime::METHOD_VNI) ? "[VNI] " : "[TLX] ";
  check_str(kp.preedit(), want, tag + std::string("go \"") + keys + "\"");
}

inline void check_telex(const char *keys, const std::string &want)
{
  check_typing_m(vietime::METHOD_TELEX, keys, want);
}

inline void check_vni(const char *keys, const std::string &want)
{
  check_typing_m(vietime::METHOD_VNI, keys, want);
}

#endif /* TYPING_H */
// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

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

  std::string out;

  for (const char *k = keys; *k; ++k)
  {

    vietime::KeyResult result = kp.handle_key(*k);

    if (result.has_commit && result.consumed)
    {
      out.append(result.commit_text);
      continue;
    }
  }

  out += kp.commit();

  const char *tag = (m == vietime::METHOD_VNI) ? "[VNI] " : "[TLX] ";
  check_str(out, want, tag + std::string("go \"") + keys + "\"");
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
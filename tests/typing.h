// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TYPING_H
#define TYPING_H

#include <string>
#include "key_processor.h"
#include "check.h"

inline void check_typing_m(vietime::InputMethod m,
                           vietime::TonePlacement p,
                           const char *keys,
                           const std::string &want)
{
  vietime::KeyProcessor kp;
  kp.set_method(m);
  kp.set_tone_placement(p);

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
  check_typing_m(vietime::METHOD_TELEX, vietime::PLACEMENT_MODERN, keys, want);
}

inline void check_vni(const char *keys, const std::string &want)
{
  check_typing_m(vietime::METHOD_VNI, vietime::PLACEMENT_MODERN, keys, want);
}

inline void check_telex_tone_placement(const char *keys, const std::string &want, vietime::TonePlacement p)
{
  check_typing_m(vietime::METHOD_TELEX, p, keys, want);
}

inline void check_vni_tone_placement(const char *keys, const std::string &want, vietime::TonePlacement p)
{
  check_typing_m(vietime::METHOD_VNI, p, keys, want);
}

#endif /* TYPING_H */
#ifndef TRANSFORM_ENGINE_H
#define TRANSFORM_ENGINE_H

#include <string>
#include "key_processor.h"

namespace vietime
{
  bool apply_modifier(std::u32string &base, char key, InputMethod method);
}

#endif
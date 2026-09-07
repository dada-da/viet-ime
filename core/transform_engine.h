#ifndef TRANSFORM_ENGINE_H
#define TRANSFORM_ENGINE_H

#include <string>
#include "input_method.h"

namespace vietime
{
  bool apply_modifier(std::u32string &base, char key, InputMethod method);
}

#endif
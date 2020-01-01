#pragma once
#include "utils/Time.h"
#include <memory>
#include <vector>

namespace MX
{
namespace Scriptable
{
    class Variable;
}

inline float exponential(float value) { return sin(value * 3.14f / 2); }
inline float inverse_exponential(float value) { return sin((1.0f - value) * 3.14f / 2); }
}
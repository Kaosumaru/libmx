#include "Easings.h"
#include <cassert>

using namespace MX;
using namespace MX::Easing;

Function MX::Easing::EasingFromString(const std::string& easing)
{
    if (easing.empty())
        return linear;

    static std::map<std::string, Function> stringToFunc = {
        { "Linear", linear<float> },

        { "QuadIn", quad<float>::in },
        { "QuadOut", quad<float>::out },
        { "QuadInOut", quad<float>::in_out },

        { "CubicIn", cubic<float>::in },
        { "CubicOut", cubic<float>::out },
        { "CubicInOut", cubic<float>::in_out },

        { "QuarticIn", quartic<float>::in },
        { "QuarticOut", quartic<float>::out },
        { "QuarticInOut", quartic<float>::in_out },

        { "QuinticIn", quintic<float>::in },
        { "QuinticOut", quintic<float>::out },
        { "QuinticInOut", quintic<float>::in_out },

        { "SinusoidalIn", sinusoidal<float>::in },
        { "SinusoidalOut", sinusoidal<float>::out },
        { "SinusoidalInOut", sinusoidal<float>::in_out },

        { "ExponentialIn", exponential<float>::in },
        { "ExponentialOut", exponential<float>::out },
        { "ExponentialInOut", exponential<float>::in_out },

        { "CircularIn", circular<float>::in },
        { "CircularOut", circular<float>::out },
        { "CircularInOut", circular<float>::in_out }

    };

    auto it = stringToFunc.find(easing);
    if (it != stringToFunc.end())
        return it->second;
    else
    {
        assert(false);
        return linear;
    }
}
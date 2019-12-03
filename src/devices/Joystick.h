#pragma once
#include "glm/vec2.hpp"
#include "utils/Signal.h"
#include "utils/SignalizingVariable.h"
#include "utils/Utils.h"
#include <memory>
#include <vector>

namespace MX
{
class Window;

class Joystick : public shared_ptr_init<Joystick>
{
public:
    using AxisState = SignalizingVariable<float>;
    using ButtonState = SignalizingVariable<bool>;

    auto& axes() { return _axes; }
    auto& buttons() { return _buttons; }

protected:
    std::vector<AxisState> _axes;
    std::vector<ButtonState> _buttons;
};

class Joysticks : public shared_ptr_init<Joysticks>
{
public:
    Joysticks();

    static pointer CreateForWindow(Window* window);

    const auto& joysticks() { return _joysticks; }

protected:
    std::vector<Joystick::pointer> _joysticks;
};

}

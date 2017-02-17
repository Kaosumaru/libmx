#include "ControlScheme.h"
#include "Collision/Area/EventsToCollisions.h"
#include "application/Window.h"
#include "scene/sprites/SpriteScene.h"
#include "widgets/Widget.h"

#include "Collision/Area/Area.h"
#include "devices/Keyboard.h"
#include "devices/Joystick.h"

using namespace MX;
using namespace MX::Game;

class KeyAction : public Action
{
public:
    KeyAction(ControlScheme* scheme, int keycode) : Action(scheme)
    {
        MX::Window::current().keyboard()->on_specific_key_up[keycode].connect([&]() { SetState(false); }, this);
        MX::Window::current().keyboard()->on_specific_key_down[keycode].connect([&]() { SetState(true); }, this);
    } 
};

class JoyButtonAction : public Action
{
public:
	JoyButtonAction(ControlScheme* scheme, unsigned joy, unsigned button) : Action(scheme)
	{
		auto &joysticks = MX::Window::current().joysticks()->joysticks();
		if (joy >= joysticks.size())
			return;
		auto &joystick = joysticks[joy];
		auto &buttons = joystick->buttons();

		if (button >= buttons.size())
			return;

		buttons[button].onValueChanged.connect([&](bool v, bool old) 
		{ 
			SetState(v); 
		}, this);
	} 
};

class JoyAxisAction : public Action
{
public:
	JoyAxisAction(ControlScheme* scheme, unsigned joy, unsigned axis, float margin) : Action(scheme)
	{
		auto &joysticks = MX::Window::current().joysticks()->joysticks();
		if (joy >= joysticks.size())
			return;
		auto &joystick = joysticks[joy];
		auto &axes = joystick->axes();

		if (axis >= axes.size())
			return;

		auto& raxis = axes[axis];

		raxis.onValueChanged.connect([this, margin](float v, float old) 
		{
			bool active = (margin >= 0.0f && v >= margin) || (margin < 0.0f && v <= margin);
			if (active)
				_weight = abs(v);
			SetState(active); 
		}, this);
	} 
};

void ControlScheme::Run()
{
    _queue.Run();
    for (auto &action : _actions)
        action->Run();
}

namespace MX
{
namespace Game
{
    Action::pointer actionForKey(int keycode)
    {
        return std::make_shared<KeyAction>(nullptr, keycode);
    }

	std::shared_ptr<Action> actionForJoystickButton(unsigned joy, unsigned button)
	{
		return std::make_shared<JoyButtonAction>(nullptr, joy, button);
	}

	std::shared_ptr<Action> actionForJoystickAxis(unsigned joy, unsigned axis, float margin)
	{
		return std::make_shared<JoyAxisAction>(nullptr, joy, axis, margin);
	}
}
}

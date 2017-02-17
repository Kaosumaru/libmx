#pragma once
#include<memory>
#include<vector>
#include "utils/Utils.h"
#include "utils/Signal.h"
#include "glm/vec2.hpp"

namespace MX
{
	class Window;

	class Joystick : public shared_ptr_init<Joystick>
	{
	public:

	protected:

	};

	class Joysticks : public shared_ptr_init<Joysticks>
	{
	public:
		Joysticks();

		static pointer CreateForWindow(Window* window);

	protected:
		std::vector<Joystick::pointer> _joysticks;
	};

}

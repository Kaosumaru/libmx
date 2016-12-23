#include "Mouse.h"
#include "EventWatcher.h"

using namespace MX;

Mouse::Mouse()
{

}

float Mouse::mouseX()
{
	return _position.x;
}
float Mouse::mouseY()
{
	return _position.y;
}
int Mouse::mouseWheel()
{
	return _wheel;
}
const glm::vec2& Mouse::position()
{
	return _position;
}

class SDLMouse : public Mouse, public SDLEventWatcher
{
public:
	SDLMouse(Window* window)
	{

	}

protected:

	void OnGenericMouse(int x, int y)
	{
		glm::vec2 pos = { x, y };
		if (_position == pos)
			return;

		auto delta = pos - _position;
		_position = pos;
		on_mouse_move(_position, delta);
	}

	void OnMouseDown(SDL_Event& event)
	{
		int index = event.button.button;
		if (index >= buttons)
			return;

		button[index] = true;
		on_mouse_button_down(_position, index);
		on_specific_button_down[index](_position);
	}

	void OnMouseUp(SDL_Event& event)
	{
		int index = event.button.button;
		if (index >= buttons)
			return;

		button[index] = false;
		on_mouse_button_up(_position, index);
		on_specific_button_up[index](_position);
	}

	void OnEvent(SDL_Event& event) override
	{
		if (event.type == SDL_MOUSEMOTION)
		{
			OnGenericMouse(event.motion.x, event.motion.y);
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			OnGenericMouse(event.button.x, event.button.y);
			OnMouseDown(event);
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{
			OnGenericMouse(event.button.x, event.button.y);
			OnMouseUp(event);
		}

		if (event.type != SDL_WINDOWEVENT)
			return;

		else if (event.window.event == SDL_WINDOWEVENT_ENTER)
		{
			on_mouse_enter(_position);
		}
		else if (event.window.event == SDL_WINDOWEVENT_LEAVE)
		{
			_position = { -1, -1 };
			on_mouse_leave(_position);
		}
	}
};


Mouse::pointer Mouse::CreateForWindow(Window* window)
{
	return std::make_shared<SDLMouse>(window);
}

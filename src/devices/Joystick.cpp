#include "Joystick.h"
#include "EventWatcher.h"

using namespace MX;

Joysticks::Joysticks()
{

}

class SDLJoystick : public Joystick, public SDLEventWatcher
{
public:
	SDLJoystick(SDL_Joystick* joy)
	{
		_joystick = joy;

		_axes.resize(SDL_JoystickNumAxes(_joystick));
		_buttons.resize(SDL_JoystickNumButtons(_joystick));

		_id = SDL_JoystickInstanceID(_joystick);
	}

	~SDLJoystick()
	{
		if (_joystick && SDL_JoystickGetAttached(_joystick))
			SDL_JoystickClose(_joystick);
	}

protected:
	void OnEvent(SDL_Event& event) override
	{
		if (event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYBUTTONUP)
		{
			SDL_JoyButtonEvent& ev = (SDL_JoyButtonEvent&)event;
			if (ev.which != _id)
				return;
			_buttons[ev.button] = ev.state == SDL_PRESSED;
		}
		else if (event.type == SDL_JOYAXISMOTION)
		{
			SDL_JoyAxisEvent& ev = (SDL_JoyAxisEvent&)event;
			if (ev.which != _id)
				return;

			float v = (float)ev.value / 32768.0f;
			_axes[ev.axis] = v;
		}
	}

	SDL_JoystickID _id;
	SDL_Joystick* _joystick;
};

class SDLJoysticks : public Joysticks, public SDLEventWatcher
{
public:
	SDLJoysticks(Window* window)
	{
		int n = SDL_NumJoysticks();
		for (int i = 0; i < n; i++)
			OpenJoystick(i);
	}

protected:
	void OpenJoystick(int i)
	{
		auto sdl_joy = SDL_JoystickOpen(i);
		auto joy = std::make_shared<SDLJoystick>(sdl_joy);
		_joysticks.push_back(joy);
	}

	void OnEvent(SDL_Event& event) override
	{
	
	}
};


Joysticks::pointer Joysticks::CreateForWindow(Window* window)
{
	return std::make_shared<SDLJoysticks>(window);
}

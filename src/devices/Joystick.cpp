#include "Joystick.h"
#include "EventWatcher.h"

using namespace MX;

Joysticks::Joysticks()
{

}

class SDLJoystick : public Joystick, public SDLEventWatcher
{
public:
	SDLJoystick(int id)
	{

	}

protected:

	void OnEvent(SDL_Event& event) override
	{

	}
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

	}

	void OnEvent(SDL_Event& event) override
	{
	
	}
};


Joysticks::pointer Joysticks::CreateForWindow(Window* window)
{
	return std::make_shared<SDLJoysticks>(window);
}

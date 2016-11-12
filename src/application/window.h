#pragma once
#include "utils/Singleton.h"
#include "utils/Utils.h"
#include "utils/Time.h"

struct SDL_Window;

namespace MX
{
class Window : public shared_ptr_init<Window>, public ScopeSingleton<Window>
{
public:
	Window(unsigned width, unsigned height, bool fullscreen = true);
	~Window();

	void Init();

	void OnRender();
	bool MainLoop();

	unsigned width() { return _width; }
	unsigned height() { return _height; }
	bool fullscreen() { return _fullscreen; }
protected:
	unsigned _width;
	unsigned _height;
	bool _fullscreen;

	SDL_Window* _window;
	std::shared_ptr<Time::Timer>       _timer;
};
}
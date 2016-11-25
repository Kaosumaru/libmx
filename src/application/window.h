#pragma once
#include "utils/Singleton.h"
#include "utils/Utils.h"
#include "utils/Time.h"
#include "glm/vec2.hpp"

struct SDL_Window;
struct SDL_Renderer;

namespace MX
{

class Mouse;
class Keyboard;
class Touches;
class MouseTouches;

class Window : public shared_ptr_init<Window>, public ScopeSingleton<Window>
{
public:
	friend class App;

	Window(unsigned width, unsigned height, bool fullscreen = true);
	~Window();

	void Init();

	unsigned width() { return _width; }
	unsigned height() { return _height; }
	glm::vec2 size() { return glm::vec2{_width, _height}; }
	bool fullscreen() { return _fullscreen; }

	const auto& mouse() { return _mouse; }
	const auto& keyboard() { return _keyboard; }
protected:
	void OnRender();
	void AfterRender();
	bool OnLoop();

	unsigned _width;
	unsigned _height;
	bool _fullscreen;

	std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> _window;

	void* _glcontext = nullptr;

	std::shared_ptr<Time::Timer>       _timer;

	std::shared_ptr<Mouse> _mouse;
	std::shared_ptr<Keyboard> _keyboard;
};
}
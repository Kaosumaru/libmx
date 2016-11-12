#include "Window.h"
#include "devices/Mouse.h"
#include "devices/Keyboard.h"
#include "SDL_video.h"

using namespace MX;

Window::Window(unsigned width, unsigned height, bool fullscreen)
{
	_width = width;
	_height = height;
	_fullscreen = fullscreen;

	Uint32 flags = SDL_WINDOW_OPENGL;
	if (_fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	_window = SDL_CreateWindow(
		"",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		_width,                               // width, in pixels
		_height,                               // height, in pixels
		flags                  // flags - see below
	);

	_mouse = Mouse::CreateForWindow(this);
	_keyboard = Keyboard::CreateForWindow(this);
}

Window::~Window()
{
	SDL_DestroyWindow(_window);
}

void Window::Init()
{

}

void Window::OnRender()
{

}

bool Window::MainLoop()
{
	return true;
}
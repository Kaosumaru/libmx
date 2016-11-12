#include "Window.h"
#include "devices/Mouse.h"
#include "devices/Keyboard.h"
#include "SDL_video.h"
#include "SDL_render.h"
#include "SDL_opengl.h"

using namespace MX;

Window::Window(unsigned width, unsigned height, bool fullscreen)
	:_window{ nullptr, SDL_DestroyWindow },
	_renderer{ nullptr, SDL_DestroyRenderer }
{
	_width = width;
	_height = height;
	_fullscreen = fullscreen;

	Uint32 flags = SDL_WINDOW_OPENGL;
	if (_fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;



	_window.reset(SDL_CreateWindow(
		"",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		_width,                               // width, in pixels
		_height,                               // height, in pixels
		flags                  // flags - see below
	));
	if (!_window) throw std::runtime_error("SDL_CreateWindow");

	//_renderer.reset(SDL_CreateRenderer(_window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
	//if (!_renderer) throw std::runtime_error("SDL_CreateRenderer");

	_mouse = Mouse::CreateForWindow(this);	
	_keyboard = Keyboard::CreateForWindow(this);

	_glcontext = SDL_GL_CreateContext(_window.get());
	if (!_renderer) throw std::runtime_error("SDL_GL_CreateContext");

	SDL_GL_SetSwapInterval(1);
}

Window::~Window()
{
	if (_glcontext)
		SDL_GL_DeleteContext(_glcontext);
}

void Window::Init()
{

}

void Window::OnRender()
{
	glClearColor ( 1.0, 0.0, 0.0, 1.0 );
	glClear ( GL_COLOR_BUFFER_BIT );
}

void Window::AfterRender()
{
	SDL_GL_SwapWindow(_window.get());
	//SDL_RenderPresent(_renderer.get());
}

bool Window::OnLoop()
{
	return true;
}	
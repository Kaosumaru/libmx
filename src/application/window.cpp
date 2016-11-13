#include "Window.h"
#include "devices/Mouse.h"
#include "devices/Keyboard.h"
#include "SDL_video.h"
#include "SDL_render.h"
#include "graphic/OpenGL.h"
#include "graphic/opengl/MVP.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace MX;

Window::Window(unsigned width, unsigned height, bool fullscreen)
	:_window{ nullptr, SDL_DestroyWindow }
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

	_mouse = Mouse::CreateForWindow(this);	
	_keyboard = Keyboard::CreateForWindow(this);

	_glcontext = SDL_GL_CreateContext(_window.get());
	if (!_glcontext) throw std::runtime_error("SDL_GL_CreateContext");

	GLenum err = glewInit();
	if (err != GLEW_OK)
		throw std::runtime_error("glewInit");

	SDL_GL_SetSwapInterval(1);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
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

	glViewport(0, 0, _width, _height);
	glm::mat4x4 mvpMat = glm::orthoLH(0.0f, (float)_width, (float)_height, 0.0f, -100.0f, 100.0f);
	gl::MVP::get().SetProjection(mvpMat);
}

void Window::AfterRender()
{
	SDL_GL_SwapWindow(_window.get());
}

bool Window::OnLoop()
{
	return true;
}	
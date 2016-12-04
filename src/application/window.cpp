#include "Window.h"
#include "devices/Mouse.h"
#include "devices/Keyboard.h"
#include "devices/Touches.h"
#include "Collision/Area/EventsToCollisions.h"
#include "Collision/Quadtree/Quadtree.h"
#include "widgets/systems/DragSystem.h"
#include "widgets/systems/TooltipSystem.h"
#include "widgets/Widget.h"

#include "SDL_video.h"
#include "SDL_render.h"
#include "graphic/OpenGL.h"
#include "graphic/Blender.h"
#include "graphic/renderer/MVP.h"
#include "graphic/renderer/Viewport.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace MX;

Window::Window(unsigned width, unsigned height, bool fullscreen)
	:_window{ nullptr, SDL_DestroyWindow }
{
	_timer = std::make_shared<Time::SimpleTimer>();

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

	auto area = std::make_shared<Collision::QuadtreeWeakLayeredArea>((float)width, (float)height);
	_windowArea = area;
	_windowArea->DefineLayerCollision(ClassID<Collision::EventsToCollisions>::id(), ClassID<Widgets::WidgetsLayer>::id());
	_windowArea->DefineLayerCollision(ClassID<Widgets::DragLayer>::id(), ClassID<Widgets::WidgetsLayer>::id());

	_mouse = Mouse::CreateForWindow(this);	
	_keyboard = Keyboard::CreateForWindow(this);
	_touches = Touches::CreateTouchesForDisplay(_timer);
	_mouseTouches = MouseTouches::CreateMouseTouchesForDisplay(_timer, _mouse);

	_dragSystem = std::make_shared<MX::Widgets::DragSystem>();
	_tooltipSystem = std::make_shared<MX::Widgets::TooltipSystem>();

	_eventsToCollisions.reset(new Collision::EventsToCollisions(_windowArea, _mouse, _touches, _mouseTouches, _dragSystem));

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
	Viewport::Set(Rectangle{0.0f, 0.0f, (float)_width, (float)_height});
	glm::mat4x4 projection = glm::orthoLH(0.0f, (float)_width, (float)_height, 0.0f, -100.0f, 100.0f);
	MVP::SetProjection(projection);
	glEnable(GL_BLEND);

	Graphic::Blender::SetCurrent(Graphic::Blender::defaultNormal());
	Graphic::Blender::defaultNormal().Apply();
}

void Window::AfterRender()
{
	SDL_GL_SwapWindow(_window.get());
}

bool Window::OnLoop()
{
	_timer->Step();
	return true;
}	
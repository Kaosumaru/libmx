#include "Application.h"
#include "Window.h"
#include "utils/Random.h"
#include <SDL.h>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace MX;

App::App()
{
	App::SetCurrent(*this);
	Time::Timer::SetCurrent(_timer);
	ScopeSingleton<FunctorsQueue, RenderQueue>::SetCurrent(_renderQueue);

	Random::Init();

	running = true;
}

App::~App()
{

}

bool App::OpenMainWindow(unsigned width, unsigned height, bool fullscreen)
{
	if (_window)
		return false;
	_window = std::make_shared<Window>(width, height, fullscreen);
	Window::SetCurrent(*_window);
	return true;
}

void App::Quit()
{
	running = false;
}

void App::Loop()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			Quit();
			break;
		}
	}

	if (_window)
		_window->OnLoop();
	OnLoop();

	if (_window)
		_window->OnRender();
	OnRender();
	if (_window)
		_window->AfterRender();
}

bool App::Run()
{
	if (!OnInit())
		return false;

	OnPrepare();

#ifdef __EMSCRIPTEN__
	auto oneIter = [](void* arg) 
	{
		((App*)arg)->Loop();
	};

	emscripten_set_main_loop_arg(oneIter, this, 0, 1);
#else
	while (running) 
	{
		Loop();
	}
#endif

	OnCleanup();
	return true;
}

void App::SetCursor(const std::shared_ptr<MX::Graphic::Image>& cursor)
{

}

void App::OnPrepare()
{

}

bool App::OnInit()
{
#ifdef __EMSCRIPTEN__
	auto flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS;
#else
	auto flags = SDL_INIT_EVERYTHING;
#endif

	if (SDL_Init(flags) != 0)
		return false;

	if (SDL_VideoInit(NULL) != 0)
	{
		SDL_Quit();
		return false;
	}
		

	return true;
}

void App::OnLoop()
{

}

void App::OnRender()
{

}

void App::OnCleanup()
{
	SDL_VideoQuit();
	SDL_Quit();
}

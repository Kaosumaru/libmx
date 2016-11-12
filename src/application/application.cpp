#include "Application.h"
#include "Window.h"
#include "utils/Random.h"
#include <SDL.h>

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

bool App::OpenWindow(unsigned width, unsigned height, bool fullscreen)
{
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

	OnLoop();
	OnRender();
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
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
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

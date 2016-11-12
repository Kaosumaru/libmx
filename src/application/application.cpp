#include "Application.h"
#include "Window.h"
#include "utils/Random.h"
#include <SDL.h>

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

bool App::Run()
{
	if (!OnInit())
		return false;

	OnPrepare();

	while (running) {
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

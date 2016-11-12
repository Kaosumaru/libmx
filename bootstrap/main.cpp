#include <stdio.h>
#include <iostream>
#include "application/application.h"
#include "application/window.h"


#include "devices/Keyboard.h"

class Bootstrap : public MX::App
{
public:
	void OnPrepare() override
	{
		OpenWindow(1280, 800, false);

		MX::Window::current().keyboard()->on_specific_key_down[SDLK_ESCAPE].connect([&]() { Quit(); });
	}
};

int main(int argc, char* argv[])
{
	Bootstrap app;
	app.Run();
	return 0;
}

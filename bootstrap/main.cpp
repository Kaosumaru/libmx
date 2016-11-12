#include <stdio.h>
#include <iostream>
#include "application/application.h"
#include "application/window.h"
#include "game/resources/Paths.h"
#include "graphic/images/Image.h"
#include "graphic/images/Texture.h"

#include "devices/Keyboard.h"

class Bootstrap : public MX::App
{
public:
	void SetResPath()
	{
#ifdef __EMSCRIPTEN__
		auto res_path = "/res/";
#else
#ifndef MX_GAME_RELEASE
		auto res_path = MX_DEBUG_RES_PATH;
#else
#if MX_PLATFORM_XCODE
		auto res_path = MX_DEBUG_RES_PATH;
#else
		auto res_path = "res/";
#endif
#endif

#ifdef MX_GAME_PERF_CHEATS
		res_path = MX_DEBUG_RES_PATH;
#endif
#endif

		MX::Paths::get().SetResourcePath(res_path);
		MX::Paths::get().SetImagePath(res_path);
		MX::Paths::get().SetSoundPath(res_path);
		MX::Paths::get().SetStreamPath(res_path);
	}

	void OnPrepare() override
	{
		SetResPath();
		OpenMainWindow(1280, 800, false);

		auto path = MX::Paths::get().pathToImage("cthulhu.png");
		_image = MX::Graphic::Texture::Create(path);
		if (_image)
		{
			std::cout << "Opened image " << _image->width() << "x" << _image->height() << std::endl;
		}

		MX::Window::current().keyboard()->on_specific_key_down[SDLK_ESCAPE].connect([&]() { Quit(); });
	}

	std::shared_ptr<MX::Graphic::Texture> _image;
};

int main(int argc, char* argv[])
{
	Bootstrap app;
	app.Run();
	return 0;
}

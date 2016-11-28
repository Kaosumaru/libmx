#include <stdio.h>
#include <iostream>
#include "application/application.h"
#include "application/window.h"
#include "game/resources/Paths.h"
#include "game/resources/Resources.h"
#include "graphic/images/Image.h"
#include "graphic/opengl/Program.h"
#include "graphic/opengl/Buffer.h"
#include "graphic/opengl/Uniform.h"
#include "graphic/renderer/MVP.h"
#include "graphic/opengl/Utils.h"

#include "devices/Keyboard.h"

#include "graphic/renderer/InstancedRenderer.h"
#include "graphic/images/TextureImage.h"
#include "utils/ListFiles.h"


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

		{
			auto path = MX::Paths::get().pathToImage("cthulhu.png");
			auto texture = MX::gl::Texture::Create(path);
			if (texture)
			{
				std::cout << "Opened image " << texture->width() << "x" << texture->height() << std::endl;
			}

			_image = std::make_shared<MX::Graphic::TextureImage>(texture);
		}

		{
			auto path = MX::Paths::get().pathToImage("");
			auto files = MX::ListFiles(path);
			for (auto& file : files)
				std::cout << file << std::endl;
		}

		MX::Window::current().keyboard()->on_specific_key_down[SDL_SCANCODE_ESCAPE].connect([&]() { Quit(); });
	}

	void OnRender() override
	{
		MX::gl::Clear({ 1.0f, 0.0f, 0.0f, 1.0f });

		/*
		if ( !_target )
		{
			_target = std::make_shared<MX::Graphic::TextureImage>(512,512);
			MX::Graphic::TargetContext context(*_target);
			MX::gl::Clear({ 1.0f, 0.0f, 1.0f, 1.0f });
			_image->DrawCentered({}, {});
		}

		_target->DrawCentered({}, {});
		*/

		_image->DrawCentered({}, {});

		//drawTriangle(_vbo, { 1.0f, 0.0f, 1.0f, 1.0f });

		{
			//auto &renderer = MX::Graphic::TextureRenderer::current();
			//renderer.Draw(*_image, MX::Rectangle{0.0f, 0.0f, 1.0f, 1.0f}, { 100.0f,100.0f }, { 0.0f, 0.0f }, { 256.0f, 256.0f }, MX::Color{}, 0.0f);
			//renderer.Draw(*_image, MX::Rectangle{0.0f, 0.0f, 0.5f, 0.5f}, { 600.0f,300.0f }, { 0.0f, 0.0f }, { 256.0f, 256.0f }, MX::Color{}, 0.0f);
		}
	}

	std::shared_ptr<MX::Graphic::TextureImage> _image;
	std::shared_ptr<MX::Graphic::TextureImage> _target;
};

int main(int argc, char* argv[])
{
	Bootstrap app;
	app.Run();
	return 0;
}

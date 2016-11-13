#include <stdio.h>
#include <iostream>
#include "application/application.h"
#include "application/window.h"
#include "game/resources/Paths.h"
#include "game/resources/Resources.h"
#include "graphic/images/Image.h"
#include "graphic/images/Texture.h"
#include "graphic/opengl/Program.h"
#include "graphic/opengl/Buffer.h"
#include "graphic/opengl/Uniform.h"
#include "graphic/opengl/MVP.h"

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

		{
			auto path = MX::Paths::get().pathToImage("cthulhu.png");
			_image = MX::Graphic::Texture::Create(path);
			if (_image)
			{
				std::cout << "Opened image " << _image->width() << "x" << _image->height() << std::endl;
			}
		}


		{
			std::string vertex = MX::Resources::get().openTextFile("shader/test.vertex");
			std::string fragment = MX::Resources::get().openTextFile("shader/test.fragment");

			std::string out;
			_program = MX::gl::createProgram(vertex, fragment, out);

			if (_program)
			{
				std::cout << "Shader compiled" << std::endl;
			}
			else
			{
				std::cout << "Shader error: " << out << std::endl;
			}

			static GLfloat vertices[] = { 250.f,  500.f, 0.0f, 
				250.f, 250.f, 0.0f,
				500.f, 250.f, 0.0f };

			_vbo.Create(MX::gl::Buffer::Type::Array);
			_vbo.Bind();
			_vbo.Data(vertices, GL_STATIC_DRAW);
		}

		MX::Window::current().keyboard()->on_specific_key_down[SDLK_ESCAPE].connect([&]() { Quit(); });
	}

	void drawTriangle(MX::gl::Buffer& buffer, const glm::vec4& c) 
	{
		_program.Use();
		GLuint posLoc = _program.GetAttribLocation("a_position");
		GLuint mvpLoc = _program.GetUniformLocation("u_mvpMatrix");
		GLuint colorLoc = _program.GetUniformLocation("u_color");

		MX::gl::Uniform(mvpLoc, MX::gl::MVP::get().mvp());
		MX::gl::Uniform(colorLoc, c);

		buffer.Bind();
		glEnableVertexAttribArray(posLoc);
		glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	void OnRender() override
	{
		drawTriangle(_vbo, { 1.0f, 0.0f, 1.0f, 1.0f });
	}

	MX::gl::Buffer  _vbo;
	MX::gl::Program _program;
	std::shared_ptr<MX::Graphic::Texture> _image;
};

int main(int argc, char* argv[])
{
	Bootstrap app;
	app.Run();
	return 0;
}

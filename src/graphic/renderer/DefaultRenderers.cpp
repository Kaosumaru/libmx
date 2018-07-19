#include "DefaultRenderers.h"
#include <iostream>
#include "graphic/opengl/Uniform.h"
#include "graphic/opengl/ProgramInstance.h"
#include "graphic/renderer/MVP.h"
#include "graphic/renderer/InstancedRenderer.h"

using namespace MX;

Graphic::Renderers::Renderers()
{
	auto createProgram = [](const char* vertexPath, const char* fragmentPath)
	{
		std::string out;
		auto program = gl::createProgramFromFiles(vertexPath, fragmentPath, out);
		if (program)
		{
			std::cout << "Shader compiled" << std::endl;
		}
		else
		{
			std::cout << "Shader error: " << out << std::endl;
		}
		return program;
	};


	{
		auto instance = std::make_shared<gl::ProgramInstance>(createProgram("shader/instanced.vertex", "shader/text.fragment"));
		_textRenderer = std::make_shared<Graphic::InstancedRenderer>(instance);
	}
    
	{
		auto instance = std::make_shared<gl::ProgramInstance>(createProgram("shader/instanced.vertex", "shader/basic.fragment"));
		_defaultRenderer = std::make_shared<Graphic::InstancedRenderer>(instance);
	}
}

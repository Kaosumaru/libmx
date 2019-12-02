#include "DefaultRenderers.h"
#include "graphic/opengl/ProgramInstance.h"
#include "graphic/opengl/Uniform.h"
#include "graphic/renderer/InstancedRenderer.h"
#include "graphic/renderer/MVP.h"
#include <iostream>

using namespace MX;

gl::Program::pointer Graphic::Renderers::createProgram(const char* fragmentPath, const char* vertexPath)
{
    std::string out;
    if (!vertexPath)
        vertexPath = defaultVertexShaderPath();
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
}

Graphic::Renderers::Renderers()
{
    {
        auto instance = std::make_shared<gl::ProgramInstance>(createProgram("shader/text.fragment"));
        _textRenderer = std::make_shared<Graphic::InstancedRenderer>(instance);
    }

    {
        auto instance = std::make_shared<gl::ProgramInstance>(createProgram("shader/basic.fragment"));
        _defaultRenderer = std::make_shared<Graphic::InstancedRenderer>(instance);
    }
}

#include "DefaultRenderers.h"
#include "graphic/opengl/ProgramInstance.h"
#include "graphic/opengl/Uniform.h"
#include "graphic/renderer/InstancedRenderer.h"
#include "graphic/renderer/MVP.h"
#include <iostream>
#include "utils/Log.h"

using namespace MX;

gl::Program::pointer Graphic::Renderers::createProgram(const char* fragmentPath, const char* vertexPath)
{
    std::string out;
    if (!vertexPath)
        vertexPath = defaultVertexShaderPath();
    auto program = gl::createProgramFromFiles(vertexPath, fragmentPath, out);
    if (program)
    {
        spdlog::info("Shader compiled: {}@{}", fragmentPath, vertexPath);
    }
    else
    {
        spdlog::error("Shader compiled: {}@{}", fragmentPath, vertexPath);
    }
    return program;
}

Graphic::Renderers::Renderers()
{
    _textRenderer = Graphic::InstancedRenderer::Create("shader/text.fragment");
    _defaultRenderer = Graphic::InstancedRenderer::Create("shader/basic.fragment");
}

#include "DefaultRenderers.h"
#include <iostream>
#include "graphic/opengl/Uniform.h"
#include "graphic/renderer/MVP.h"
#include "graphic/renderer/InstancedRenderer.h"

using namespace MX;

Graphic::Renderers::Renderers()
{
    _textRenderer = std::make_shared<Graphic::InstancedRenderer>("shader/instanced.vertex", "shader/text.fragment");
	_defaultRenderer = std::make_shared<Graphic::InstancedRenderer>();
}

#pragma once
#include "TextureRenderer.h"
#include <string>

namespace MX{
namespace Graphic
{
    class Renderers : public DeinitSingleton<Renderers>
    {
    public:
        Renderers();

        auto& textRenderer() { return *_textRenderer; }
        auto& defaultRenderer() { return *_defaultRenderer; }
    protected:
		std::shared_ptr<MX::Graphic::TextureRenderer> _defaultRenderer;
        std::shared_ptr<MX::Graphic::TextureRenderer> _textRenderer;
    };
}
}

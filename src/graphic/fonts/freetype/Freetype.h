#pragma once
#include<memory>
#include<string>

namespace MX
{
namespace Graphic
{
    class TextureImage;

    struct FreetypeUtils
    {
        static std::shared_ptr<Graphic::TextureImage> drawLine(const std::string& text);
    };
}
}

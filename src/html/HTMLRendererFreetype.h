#pragma once
#include <memory>
#include <string>

namespace MX
{
namespace Graphic
{
    class TextureImage;
    class Font;
}

class HTMLRendererFreetype
{
public:
    static std::shared_ptr<Graphic::TextureImage> DrawOnBitmap(const char* str, int width, const std::shared_ptr<Graphic::Font>& defaultFont = nullptr);
    static std::shared_ptr<Graphic::TextureImage> DrawOnBitmap(const std::wstring& str, int width, const std::shared_ptr<Graphic::Font>& defaultFont = nullptr);
};

}

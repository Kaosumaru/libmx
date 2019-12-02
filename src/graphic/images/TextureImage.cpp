#include "TextureImage.h"
#include "graphic/opengl/Utils.h"
#include "graphic/renderer/TextureRenderer.h"

using namespace MX;
using namespace MX::Graphic;

TextureImage::~TextureImage()
{
    _texture = nullptr;
}

TextureImage::TextureImage(const TextureImage& parent, float x, float y, float w, float h)
{
    _texture = parent._texture;
    _dimensions = parent.subDimensions(x, y, w, h);

    auto coord1 = dimensionToUV({ _dimensions.x, _dimensions.y });
    auto coord2 = dimensionToUV({ _dimensions.x + w, _dimensions.y + h });
    _uvCoords.x1 = coord1.x;
    _uvCoords.y1 = coord1.y;
    _uvCoords.x2 = coord2.x;
    _uvCoords.y2 = coord2.y;
}

glm::vec2 TextureImage::dimensionToUV(const glm::vec2& v)
{
    return v / _texture->size();
}

glm::vec4 TextureImage::subDimensions(float x, float y, float w, float h) const
{
    glm::vec4 otherDimensions;

    otherDimensions.x = _dimensions.x + x;
    otherDimensions.y = _dimensions.y + y;

    otherDimensions.z = w;
    otherDimensions.w = h;
    return otherDimensions;
}

TextureImage::TextureImage(unsigned width, unsigned height, bool alpha)
{
    int w = width;
    int h = height;
    glm::vec2 uv = { 1.0f, 1.0f };

#if 0
#ifdef __EMSCRIPTEN__
	w = gl::UpperPowerOfTwo(width);
	h = gl::UpperPowerOfTwo(height);
#endif
#endif

    _texture = std::make_shared<gl::Texture>(glm::ivec2 { w, h }, alpha);
    _dimensions = { 0.0f, 0.0f, (float)width, (float)height };

    uv = dimensionToUV({ (float)width, (float)height });
    _uvCoords = Rectangle { 0.0f, 0.0f, uv.x, uv.y };
}

TextureImage::TextureImage(const TextureImage& parent, const MX::Rectangle& rect)
    : TextureImage(parent, rect.x1, rect.y1, rect.width(), rect.height())
{
}

TextureImage::TextureImage(const TexturePointer& texture)
{
    SetTexture(texture);
}

void TextureImage::SetTexture(const TexturePointer& texture)
{
    if (!texture)
        return;
    _texture = texture;
    _dimensions = { 0.0f, 0.0f, texture->size() };
    _uvCoords = Rectangle { 0.0f, 0.0f, 1.0f, 1.0f };
}

TextureImage::pointer TextureImage::Create(const SurfaceRGBA& surface)
{
    auto txt = std::make_shared<gl::Texture>(surface.width(), surface.height(), GL_RGBA, GL_RGBA, surface.data());
    return std::make_shared<TextureImage>(txt);
}

TextureImage::pointer TextureImage::Create(const SurfaceGrayscale& surface)
{
    auto txt = std::make_shared<gl::Texture>(surface.width(), surface.height(), GL_ALPHA, GL_ALPHA, surface.data());
    return std::make_shared<TextureImage>(txt);
}

TextureImage::pointer TextureImage::Create(unsigned width, unsigned height, unsigned int dataFormat, void* data)
{
    auto txt = std::make_shared<gl::Texture>(width, height, dataFormat, dataFormat, data);
    return std::make_shared<TextureImage>(txt);
}

TextureImage::pointer TextureImage::Create(const std::string& path)
{
    auto texture = MX::gl::Texture::Create(path);
    if (!texture)
        return nullptr;

    return std::make_shared<MX::Graphic::TextureImage>(texture);
}

void TextureImage::Clear(const Color& color)
{
    TargetContext c(*this);
    gl::Clear(color);
}

void TextureImage::DrawCentered(const glm::vec2& offset, const glm::vec2& pos, const glm::vec2& scale, float angle, const Color& color)
{
    auto& calculated_color = Settings::modifyColor(color);
    Graphic::TextureRenderer::current().Draw(*_texture, _uvCoords, pos, (offset + _center) * scale, glm::vec2 { scale.x * _dimensions.z, scale.y * _dimensions.w }, calculated_color, angle);
}

void TextureImage::DrawArea(const MX::Rectangle& destination, const MX::Rectangle& source, const Color& color)
{
    auto dimensions = subDimensions(source.x1, source.y1, source.width(), source.height());
    Rectangle uvCoords;

    auto coord1 = dimensionToUV({ dimensions.x, dimensions.y });
    auto coord2 = dimensionToUV({ dimensions.x + source.width(), dimensions.y + source.height() });
    uvCoords.x1 = coord1.x;
    uvCoords.y1 = coord1.y;
    uvCoords.x2 = coord2.x;
    uvCoords.y2 = coord2.y;

    glm::vec2 pos = { destination.x1, destination.y1 };
    auto& calculated_color = Settings::modifyColor(color);
    Graphic::TextureRenderer::current().Draw(*_texture, uvCoords, { pos.x, pos.y }, { 0.0f, 0.0f }, { destination.width(), destination.height() }, color, 0.0f);
}

unsigned TextureImage::Height()
{
    return (unsigned)_dimensions.w;
}
unsigned TextureImage::Width()
{
    return (unsigned)_dimensions.z;
}

bool TextureImage::empty()
{
    return _texture ? false : true;
}

bool TextureImage::save(const std::string& path)
{
    //ci::writeImage(path, _texture->createSource());
    return false;
}

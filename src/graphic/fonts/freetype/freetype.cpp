#include "Freetype.h"
#include "game/resources/Paths.h"
#include "graphic/images/TextureImage.h"
#include "graphic/opengl/Utils.h"
#include "utils/Log.h"
#include <map>
#include <memory>

using namespace MX;

namespace
{
int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return numToRound + multiple - remainder;
}

template<typename T>
std::shared_ptr<Graphic::TextureImage> _drawLine(const std::shared_ptr<Graphic::Face>& face, const T& text)
{
    int ascender = face->scaled_ascender();
    auto descender = face->scaled_descender();
    int width = Graphic::FreetypeUtils::measureLine(face, text);

    int height = ascender - descender;

    
    width = roundUp(width, 4);
    height = roundUp(height, 4);

#ifdef _DEBUG
    bool offSurfaceRendered = false;
#endif
    Graphic::SurfaceGrayscale surface(width, height);
    FT_Vector pen = { 0, ascender };
    face->draw_text(text.c_str(), pen, [&](int x, int y, uint8_t p) {
        if (surface.contains(x, y))
            surface.at(x, y) += p;
        else
        {
#ifdef _DEBUG
            if (offSurfaceRendered)
                return;
            offSurfaceRendered = true;
            spdlog::warn("FreetypeUtils::drawLine drawing outside boundaries!");
#endif
        }
    });

    auto texture = Graphic::TextureImage::Create(surface);
    float center_y = 0.0f;
    texture->SetCenter({ 0.0f, center_y });
    return texture;
}
}

class FontManager : public DeinitSingleton<FontManager>
{
public:
    using FontInfo = std::tuple<std::string, unsigned>;

    auto& createFont(const std::string& path, unsigned size)
    {
        auto& font = _font[std::make_tuple(path, size)];
        if (!font)
            font = std::make_shared<Graphic::Face>(path, size);
        return font;
    }

    ~FontManager()
    {
        for (auto& f : _font)
            f.second->Clear();
    }

protected:
    std::map<FontInfo, std::shared_ptr<Graphic::Face>> _font;
};

Graphic::Freetype::~Freetype()
{
    FontManager::Deinit();
    FT_Done_FreeType(_library);
}

std::shared_ptr<Graphic::Face> Graphic::Face::Create(const std::string& path, unsigned size)
{
    return FontManager::get().createFont(path, size);
}

std::shared_ptr<Graphic::TextureImage> Graphic::FreetypeUtils::drawLine(const std::shared_ptr<Face>& face, const std::string& text)
{
    return _drawLine(face, text);
}

std::shared_ptr<Graphic::TextureImage> Graphic::FreetypeUtils::drawLine(const std::shared_ptr<Face>& face, const std::wstring& text)
{
    return _drawLine(face, text);
}

int Graphic::FreetypeUtils::measureLine(const std::shared_ptr<Face>& face, const std::string& text)
{
    FT_Vector pen = { 0, 0 };
    return face->draw_text(text.c_str(), pen, [&](int x, int y, uint8_t p) {});
}

int Graphic::FreetypeUtils::measureLine(const std::shared_ptr<Face>& face, const std::wstring& text)
{
    FT_Vector pen = { 0, 0 };
    return face->draw_text(text.c_str(), pen, [&](int x, int y, uint8_t p) {});
}
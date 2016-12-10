#include "Freetype.h"
#include "graphic/images/TextureImage.h"
#include "game/resources/Paths.h"
#include <memory>
#include <map>
#include "graphic/opengl/Utils.h"

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
}


class FontManager : public DeinitSingleton<FontManager>
{
public:
	using FontInfo = std::tuple<std::string, unsigned>;

	auto& createFont(const std::string& path, unsigned size)
	{
		auto &font = _font[std::make_tuple(path, size)];
		if (!font)
			font = std::make_shared<Graphic::Face>(path, size);
		return font;
	}

protected:
	std::map<FontInfo, std::shared_ptr<Graphic::Face>> _font;
};

std::shared_ptr<Graphic::Face> Graphic::Face::Create(const std::string& path, unsigned size)
{
	return FontManager::get().createFont(path, size);
}

std::shared_ptr<Graphic::TextureImage> Graphic::FreetypeUtils::drawLine( const std::shared_ptr<Face>& face, const std::string& text)
{
	int ascender = face->face()->ascender >> 6;
    FT_Vector     pen = { 0, ascender };

	int width = measureLine(face, text);
	int height = ascender - (face->face()->descender >> 6);
	width = roundUp(width, 4);
	height = roundUp(height, 4);

	Graphic::SurfaceGrayscale surface( width, height );
    face->draw_text( text.c_str(), pen, [&](int x, int y, uint8_t p) 
	{
		surface.at( x, y ) += p;
    } );
	return Graphic::TextureImage::Create(surface);
}


std::shared_ptr<Graphic::TextureImage> Graphic::FreetypeUtils::drawLine( const std::shared_ptr<Face>& face, const std::wstring& text)
{
	int ascender = face->face()->ascender >> 6;
    FT_Vector     pen = { 0, ascender };

	int width = measureLine(face, text);
	int height = ascender - (face->face()->descender >> 6);
	width = roundUp(width, 4);
	height = roundUp(height, 4);

	Graphic::SurfaceGrayscale surface( width, height );
    face->draw_text( text.c_str(), pen, [&](int x, int y, uint8_t p) 
	{
		if (surface.contains(x,y))
			surface.at( x, y ) += p;
    } );
	return Graphic::TextureImage::Create(surface);
}

int Graphic::FreetypeUtils::measureLine( const std::shared_ptr<Face>& face, const std::string& text )
{
    FT_Vector     pen = { 0, 0 };
    return face->draw_text( text.c_str(), pen, [&]( int x, int y, uint8_t p ) {});
}

int Graphic::FreetypeUtils::measureLine( const std::shared_ptr<Face>& face, const std::wstring& text )
{
    FT_Vector     pen = { 0, 0 };
    return face->draw_text( text.c_str(), pen, [&]( int x, int y, uint8_t p ) {});
}
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



std::shared_ptr<Graphic::TextureImage> Graphic::FreetypeUtils::drawLine( const std::shared_ptr<Face>& face, const std::string& text)
{
	int ascender = face->face()->ascender >> 6;
    FT_Vector     pen = { 0, ascender };

	for ( auto &c : text )
	{
		auto& glyph = face->LoadCharCached( c );
		pen.x += glyph.glyph()->advance.x >> 16;
	}

	int width = pen.x;
	int height = ascender - (face->face()->descender >> 6);
#ifdef __EMSCRIPTEN__
	width = gl::UpperPowerOfTwo(width);
	height = gl::UpperPowerOfTwo(height);
#else
    width = roundUp(width, 4);
    height = roundUp(height, 4);
#endif
    
	pen.x = 0;

	{
		Graphic::SurfaceGrayscale surface( width, height );

		for ( auto &c : text )
		{
			auto& glyph = face->LoadCharCached( c );

			auto advance_x = glyph.iterate_bitmap( [&](int x, int y, uint8_t p) 
			{
				surface.at( pen.x + x, pen.y + y ) += p;
			});

			pen.x += advance_x >> 16;
		}
		return Graphic::TextureImage::Create(surface);
	}

}


std::shared_ptr<Graphic::TextureImage> Graphic::FreetypeUtils::drawLine( const std::shared_ptr<Face>& face, const std::wstring& text)
{
	int ascender = face->face()->ascender >> 6;
    FT_Vector     pen = { 0, ascender };

	for ( auto &c : text )
	{
		auto& glyph = face->LoadCharCached( c );
		pen.x += glyph.glyph()->advance.x >> 16;
	}

	int width = pen.x;
	int height = ascender - (face->face()->descender >> 6);
#ifdef __EMSCRIPTEN__
	width = gl::UpperPowerOfTwo(width);
	height = gl::UpperPowerOfTwo(height);
#else
    width = roundUp(width, 4);
    height = roundUp(height, 4);
#endif

	pen.x = 0;

	{
		Graphic::SurfaceGrayscale surface( width, height );

		for ( auto &c : text )
		{
			auto& glyph = face->LoadCharCached( c );

			auto advance_x = glyph.iterate_bitmap( [&](int x, int y, uint8_t p) 
			{
				surface.at( pen.x + x, pen.y + y ) += p;
			});

			pen.x += advance_x >> 16;
		}

		return Graphic::TextureImage::Create(surface);
	}



}
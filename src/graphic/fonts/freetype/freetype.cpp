#include "Freetype.h"
#include "graphic/images/TextureImage.h"
#include "game/resources/Paths.h"
#include <memory>
#include <map>
#include "graphic/opengl/Utils.h"

using namespace MX;



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
    width += width % 2;
    height += height % 2;
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
    width += width % 2;
    height += height % 2;
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

        for ( int y = 0; y < height; y++ )
            surface.at( width - 1, y ) = 255;

		return Graphic::TextureImage::Create(surface);
	}



}
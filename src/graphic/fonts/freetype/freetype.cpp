#include "freetype.h"
#include "graphic/images/TextureImage.h"
#include "game/resources/Paths.h"
#include <memory>
#include <map>

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
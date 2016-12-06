#include "freetype.h"
#include "utils/Singleton.h"
#include "graphic/images/TextureImage.h"
#include "game/resources/Paths.h"
#include <memory>
#include <map>


#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

using namespace MX;

class Freetype : public DeinitSingleton<Freetype>
{
public:
	Freetype()
	{
		FT_Init_FreeType(&_library);
		//error
	}

	~Freetype()
	{
		FT_Done_FreeType(_library);
	}

	static auto& library()
	{
		return Freetype::get()._library;
	}

protected:
	FT_Library  _library;
};


class Glyph
{
public:
    Glyph() {}
    Glyph( const Glyph& other ) = delete;

    auto& glyph() { return _glyph; }

    template<typename T>
    auto iterate_bitmap( T&& t )
    {
        auto g = (FT_BitmapGlyph)_glyph;
        int ox = g->left;
        int oy = -g->top;

        FT_Int  p, q;
        FT_Int  x_max = g->bitmap.width;
        FT_Int  y_max = g->bitmap.rows;

        auto pointer = g->bitmap.buffer;
        for ( q = oy; q < y_max + oy; q++ )
        {
            for ( p = ox; p < x_max + ox; p++ )
            {
                t( p, q, *pointer );
                pointer++;
            }
        }

        return g->root.advance.x;
    }

    ~Glyph()
    {
        if (_glyph)
            FT_Done_Glyph(_glyph);
    }
protected:
    FT_Glyph   _glyph = nullptr;
};

class Face
{
public:
	Face(const std::string& path)
	{
		auto error = FT_New_Face(Freetype::library(), path.c_str(), 0, &_face);
		//WIPerror
	}

	~Face()
	{
		FT_Done_Face(_face);
	}

	void SetCharSize(FT_F26Dot6 width, FT_F26Dot6 height = 0, FT_UInt hres = 100, FT_UInt vres = 0)
	{
		auto error = FT_Set_Char_Size(_face, width, height, hres, vres);
		//WIPerror
	}

	Glyph& LoadCharCached(FT_ULong c)
	{
        auto &cached_glyph = _charToGlyph[c];
        if (!cached_glyph.glyph() && LoadChar(c))
        {
            auto slot = glyph_slot();
            FT_Get_Glyph( slot, &cached_glyph.glyph() );
        }

        return cached_glyph;
	}

protected:
    FT_GlyphSlot glyph_slot()
    {
        return _face->glyph;
    }

    FT_ULong GetCharIndex(FT_ULong c)
    {
        return FT_Get_Char_Index( _face, c );
    }

	bool LoadChar(FT_ULong c)
	{
		return FT_Load_Char( _face, c, FT_LOAD_RENDER ) == 0;
	}

    std::map<FT_ULong, Glyph> _charToGlyph;
	FT_Face _face;
};

namespace
{
    template<typename T>
    void iterate_bitmap( FT_Bitmap* bitmap, int ox, int oy, T&& t )
    {
        FT_Int  p, q;
        FT_Int  x_max = bitmap->width;
        FT_Int  y_max = bitmap->rows;

        auto pointer = bitmap->buffer;
        for ( q = oy; q < y_max + oy; q++ )
        {
            for ( p = ox; p < x_max + ox; p++ )
            {
                t( p, q, *pointer );
                pointer++;
            }
        }

    }
}


std::shared_ptr<Graphic::TextureImage> Graphic::FreetypeUtils::testText( const std::string& text )
{
    Graphic::SurfaceRGBA surface( 512, 512 );
    surface.Clear({ 0, 0, 0, 0 });

    auto path = Paths::get().pathToResource("font/arial.ttf");
    auto face = std::make_shared<Face>(path);

    int size = 24;

    face->SetCharSize(size << 6);

    FT_Vector     pen = { 200, 300 };

    for ( auto &c : text )
    {
        auto& glyph = face->LoadCharCached( c );

        auto advance_x = glyph.iterate_bitmap( [&](int x, int y, uint8_t p) 
        {
            surface.at( pen.x + x, pen.y + y ).a = p;
        });

        pen.x += advance_x >> 16;
    }




    return Graphic::TextureImage::Create(surface);
}

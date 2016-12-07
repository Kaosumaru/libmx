#pragma once
#include "utils/Singleton.h"
#include<memory>
#include<string>
#include<map>
#include "glm/vec4.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H


namespace MX
{
namespace Graphic
{

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

		auto face() { return _face; }

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


    class TextureImage;

    struct FreetypeUtils
    {
        static std::shared_ptr<Graphic::TextureImage> drawLine(const std::shared_ptr<Face>& face, const std::string& text);
    };
}
}

#include "freetype.h"
#include "utils/Singleton.h"

#include <ft2build.h>
#include FT_FREETYPE_H

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

class Face
{
public:
	Face(const std::string& path)
	{
		FT_New_Face(Freetype::library(), path.c_str(), 0, &_face);
		//error
	}

	~Face()
	{
		FT_Done_Face(_face);
	}

	void SetCharSize(FT_F26Dot6 width, FT_F26Dot6 height = 0, FT_UInt hres = 100, FT_UInt vres = 0)
	{
		FT_Set_Char_Size(_face, width, height, hres, vres);
		//error
	}

	void LoadChar(FT_ULong c)
	{
		FT_Load_Char( _face, c, FT_LOAD_RENDER );
	}

protected:
	FT_Face _face;
};
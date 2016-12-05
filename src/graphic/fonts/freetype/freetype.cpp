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
		FT_Init_FreeType( &_library );
	}

	~Freetype()
	{
		FT_Done_FreeType( _library );
	}

protected:
	FT_Library  _library;
};
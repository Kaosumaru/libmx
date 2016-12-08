#include "Font.h"
#include "utils/Singleton.h"
#include "graphic/images/TextureImage.h"
#include "game/resources/Paths.h"
#include <memory>
#include <map>
#include "graphic/fonts/freetype/Freetype.h"

using namespace MX;
using namespace MX::Graphic;


Font::Font(const std::string& path, float size, const std::string& face_name) :_path(path), _size(size), _face_name(face_name)
{
	_ftFace = std::make_shared<Face>(path);
	_ftFace->SetCharSize((int)size << 6);
#ifdef WIP
	//cache faces, deinit glyphs before end
#endif
}
Font::~Font()
{

}

Font::pointer Font::Create(const std::string& path, float size, const std::string& face_name)
{
	return std::make_shared<Font>(Paths::get().pathToResource(path), size, face_name);
}
Font::pointer Font::CreateDefault()
{
	return Create("font/arial.ttf", 16.0f);
}

std::shared_ptr<TextureImage> Font::DrawTextOnBitmap(const std::string &str)
{
	return FreetypeUtils::drawLine(_ftFace, str);
}
std::shared_ptr<TextureImage> Font::DrawTextOnBitmap(const std::wstring &str)
{
	return nullptr;
}

bool Font::empty()
{
	return !_ftFace;
}

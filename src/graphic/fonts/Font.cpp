#include "Font.h"
#include "utils/Singleton.h"
#include "graphic/images/TextureImage.h"
#include "game/resources/Paths.h"
#include <memory>
#include <map>

using namespace MX;
using namespace MX::Graphic;


Font::Font(const std::string& path, float size, const std::string& face_name) :_path(path), _size(size), _face_name(face_name)
{

}
Font::~Font()
{

}

Font::pointer Font::Create(const std::string& path, float size, const std::string& face_name)
{
	return nullptr;
}
Font::pointer Font::CreateDefault()
{
	return Create("font/arial.ttf", 16.0f);
}

std::shared_ptr<TextureImage> Font::DrawTextOnBitmap(const std::string &str, const Color &color)
{
	return nullptr;
}
std::shared_ptr<TextureImage> Font::DrawTextOnBitmap(const std::wstring &str, const Color &color)
{
	return nullptr;
}

bool Font::empty()
{
	return true;
	//return !_ftFace;
}

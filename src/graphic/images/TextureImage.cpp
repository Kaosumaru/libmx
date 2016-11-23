#include "TextureImage.h"
#include "graphic/renderer/TextureRenderer.h"
#include "graphic/opengl/Utils.h"

using namespace MX;
using namespace MX::Graphic;




TextureImage::~TextureImage()
{
	_texture = nullptr;
}

TextureImage::TextureImage(const TextureImage& parent, float x, float y, float w, float h)
{
	_texture = parent._texture;
	_dimensions = parent.subDimensions(x, y, w, h);

	auto coord1 = dimensionToUV({_dimensions.x, _dimensions.y});
	auto coord2 = dimensionToUV({_dimensions.x + w, _dimensions.y + h});
	_uvCoords.x1 = coord1.x;
	_uvCoords.y1 = coord1.y;
	_uvCoords.x2 = coord2.x;
	_uvCoords.y2 = coord2.y;
}

glm::vec2 TextureImage::dimensionToUV(const glm::vec2& v)
{
	return v / _texture->size();
}

glm::vec4 TextureImage::subDimensions(float x, float y, float w, float h) const
{
	glm::vec4 otherDimensions;

	otherDimensions.x = _dimensions.x + x;
	otherDimensions.y = _dimensions.y + y;

	otherDimensions.z = w;
	otherDimensions.w = h;
	return otherDimensions;
}


TextureImage::TextureImage(unsigned width, unsigned height, bool alpha)
{
	_texture = std::make_shared<gl::Texture>(glm::ivec2{ width, height }, alpha);
	_dimensions = { 0.0f, 0.0f, _texture->size() };
	_uvCoords = Rectangle{ 0.0f, 0.0f, 1.0f, 1.0f };
}

TextureImage::TextureImage(const TextureImage& parent, const MX::Rectangle& rect) : TextureImage(parent, rect.x1, rect.y1, rect.width(), rect.height())
{

}

TextureImage::TextureImage(const TexturePointer& texture)
{
	SetTexture(texture);
}


void TextureImage::SetTexture(const TexturePointer& texture)
{
	if (!texture)
		return;
	_texture = texture;
	_dimensions = { 0.0f, 0.0f, texture->size() };
	_uvCoords = Rectangle{ 0.0f, 0.0f, 1.0f, 1.0f };
}


TextureImage::pointer TextureImage::Create(unsigned width, unsigned height, unsigned int dataFormat, void *data)
{
	auto txt = std::make_shared<gl::Texture>(width, height, dataFormat, data);
	return std::make_shared<TextureImage>(txt);
}

void TextureImage::Clear(const Color &color)
{
	TargetContext c(*this);
	gl::Clear(color);
}

void TextureImage::DrawCentered(const glm::vec2& offset, const glm::vec2& pos, const glm::vec2& scale, float angle, const Color &color)
{
	auto &calculated_color = Settings::modifyColor(color);
	Graphic::TextureRenderer::current().Draw(*_texture, _uvCoords, pos, offset + _center, glm::vec2{ scale.x * _dimensions.z, scale.y * _dimensions.w }, calculated_color, angle);
}

void TextureImage::Draw(const MX::Rectangle &destination, const MX::Rectangle &source, const Color &color)
{
	auto dimensions = subDimensions(source.x1, source.y1, source.width(), source.height());
	Rectangle uvCoords;

	auto coord1 = dimensionToUV({_dimensions.x, _dimensions.y});
	auto coord2 = dimensionToUV({_dimensions.x + source.width(), _dimensions.y + source.height()});
	uvCoords.x1 = coord1.x;
	uvCoords.y1 = coord1.y;
	uvCoords.x2 = coord2.x;
	uvCoords.y2 = coord2.y;

	glm::vec2 pos = { destination.x1, destination.y1 };
	auto &calculated_color = Settings::modifyColor(color);
	Graphic::TextureRenderer::current().Draw(*_texture, uvCoords, { pos.x, pos.y }, { 0.0f, 0.0f }, { destination.width(), destination.height() }, color, 0.0f);
}

unsigned TextureImage::Height()
{
	return (unsigned)_dimensions.z;
}
unsigned TextureImage::Width()
{
	return (unsigned)_dimensions.w;
}


bool TextureImage::empty()
{
	return _texture ? false : true;
}

bool TextureImage::save(const std::string &path)
{
	//ci::writeImage(path, _texture->createSource());
	return false;
}

#include "Surface.h"
#include "graphic/renderer/TextureRenderer.h"

using namespace MX;
using namespace MX::Graphic;




Surface::~Surface()
{
	_texture = nullptr;
}

Surface::Surface(const Surface& parent, float x, float y, float w, float h)
{
	_texture = parent._texture;
	_texCoords = parent.subCoords(x, y, w, h);
}

Rectangle Surface::subCoords(float x, float y, float w, float h) const
{
	Rectangle otherCoords;

	otherCoords.x1 = _texCoords.x1 + x;
	otherCoords.y1 = _texCoords.y1 + y;

	otherCoords.x2 = otherCoords.x1 + w;
	otherCoords.y2 = otherCoords.y1 + h;
	return otherCoords;
}


Surface::Surface(unsigned width, unsigned height, bool alpha)
{
	assert(false);
	//WIP
	/*
	ci::gl::Texture::Format format;
	format.setInternalFormat(alpha ? GL_RGBA : GL_RGB);
	SetTexture(ci::gl::Texture::create(width, height, format));
	*/
}

Surface::Surface(const Surface& parent, const MX::Rectangle& rect) : Surface(parent, rect.x1, rect.y1, rect.width(), rect.height())
{

}

Surface::Surface(const TexturePointer& texture)
{
	SetTexture(texture);
}


void Surface::SetTexture(const TexturePointer& texture)
{
	if (!texture)
		return;
	_texture = texture;
	_texCoords = Rectangle{ 0.0f, 0.0f, 1.0f, 1.0f };
}


Surface::pointer Surface::Create(void *data, unsigned int dataFormat, unsigned width, unsigned height)
{
#if 0
	ci::gl::Texture::Format format;
	format.setInternalFormat(GL_RGBA);
	format.loadTopDown();
	auto txt = ci::gl::Texture::create(data, dataFormat, width, height, format);
	return std::make_shared<Surface>(txt);
#endif
	assert(false);
	//WIP
	return nullptr;
}






void Surface::Clear(const Color &color)
{
	TargetContext c(*this);
	assert(false);
	//ci::gl::clear(color.toColorA(), false);
}

void Surface::DrawCentered(const glm::vec2& offset, const glm::vec2& pos, const glm::vec2& scale, float angle, const Color &color)
{
	auto &calculated_color = Settings::modifyColor(color);
	Graphic::TextureRenderer::current().Draw(*_texture, _texCoords, pos, offset + _center, scale * glm::vec2{ dimensions() }, calculated_color, angle);
}

void Surface::Draw(const MX::Rectangle &destination, const MX::Rectangle &source, const Color &color)
{
	glm::vec2 pos = { destination.x1, destination.y1 };
	auto &calculated_color = Settings::modifyColor(color);
	auto coords = subCoords(source.x1, source.y1, source.width(), source.height());
	Graphic::TextureRenderer::current().Draw(*_texture, coords, { pos.x, pos.y }, { 0.0f, 0.0f }, { destination.width(), destination.height() }, color, 0.0f);
}

unsigned Surface::Height()
{
	return (unsigned)(_texCoords.height() * _texture->height());
}
unsigned Surface::Width()
{
	return (unsigned)(_texCoords.width() * _texture->width());
}


bool Surface::empty()
{
	return _texture ? false : true;
}

bool Surface::save(const std::string &path)
{
	//ci::writeImage(path, _texture->createSource());
	return false;
}

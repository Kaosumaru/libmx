#if 0
#include "Surface.h"
#include "Utils/MXQuad.h"
//#include "Graphic/Renderers/MXTextureRenderer.h"

using namespace MX;
using namespace MX::Graphic;




Surface::~Surface()
{
	_texture = nullptr;
}

Surface::Surface(const Surface& parent, int x, int y, int w, int h)
{
	_texture = parent._texture;
	_texCoords = parent.subCoords(x, y, w, h);

}

ci::Area Surface::subCoords(int x, int y, int w, int h) const
{
	ci::Area otherCoords;

	otherCoords.x1 = _texCoords.x1 + x;
	otherCoords.y1 = _texCoords.y1 + y;

	otherCoords.x2 = otherCoords.x1 + w;
	otherCoords.y2 = otherCoords.y1 + h;
	return otherCoords;
}


Surface::Surface(unsigned width, unsigned height, bool alpha)
{
	ci::gl::Texture::Format format;
	format.setInternalFormat(alpha ? GL_RGBA : GL_RGB);
	SetTexture(ci::gl::Texture::create(width, height, format));
}

Surface::Surface(const Surface& parent, const MX::Rectangle& rect) : Surface(parent, (int)rect.x1, (int)rect.y1, (int)rect.width(), (int)rect.height())
{

}

Surface::Surface(const ci::gl::TextureRef &texture)
{
	SetTexture(texture);
}


void Surface::SetTexture(const ci::gl::TextureRef& texture)
{
	if (!texture)
		return;
	_texture = texture;
	_texCoords = _texture->getBounds();
}


Surface::pointer Surface::Create(void *data, GLenum dataFormat, unsigned width, unsigned height)
{
	ci::gl::Texture::Format format;
	format.setInternalFormat(GL_RGBA);
	format.loadTopDown();
	auto txt = ci::gl::Texture::create(data, dataFormat, width, height, format);
	return std::make_shared<Surface>(txt);
}






void Surface::Clear(const Color &color)
{
	TargetContext c(*this);
	ci::gl::clear(color.toColorA(), false);
}

void Surface::DrawCentered(float cx, float cy, float x, float y, float sx, float sy, float angle, const Color &color)
{
	auto &calculated_color = Settings::modifyColor(color);
	auto &calculated_sx = Settings::modifyScaleX(sx);
	auto &calculated_sy = Settings::modifyScaleY(sy);

	Graphic::TextureRenderer::current().Draw(_texture, _texCoords, { x, y }, { (cx + _center.x) * calculated_sx, (cy + _center.y) * calculated_sy }, { sx * Width(), sy * Height() }, calculated_color, angle);
}

void Surface::Draw(float x, float y)
{
	Image::Draw(x,y);
}


void Surface::Draw(const MX::Rectangle &destination, const MX::Rectangle &source, const Color &color)
{
	glm::vec2 pos = { destination.x1, destination.y1 };
	auto &calculated_color = Settings::modifyColor(color);
	auto coords = subCoords(source.x1, source.y1, source.width(), source.height());
	Graphic::TextureRenderer::current().Draw(_texture, coords, { pos.x, pos.y }, { 0.0f, 0.0f }, { destination.width(), destination.height() }, color, 0.0f);
}

unsigned Surface::Height()
{
	return _texCoords.getHeight();
}
unsigned Surface::Width()
{
	return _texCoords.getWidth();
}


bool Surface::empty()
{
	return _texture ? false : true;
}

bool Surface::save(const std::string &path)
{
	ci::writeImage(path, _texture->createSource());
	return true;
}

#endif
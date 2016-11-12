#pragma once
#include "Image.h"

#if 0
namespace MX{

struct Quad;

namespace Graphic
{

class Display;

class Surface : public Image, public TargetSurface, public disable_copy_constructors, public shared_ptr_init<Surface>
{
public:
	Surface(const Surface& parent, int x, int y, int w, int h);
	Surface(const Surface& parent, const MX::Rectangle& rect);
	Surface(unsigned width, unsigned height, bool alpha = true);

	Surface(const ci::gl::TextureRef &texture);
	~Surface();


	void Clear(const Color &color);

	void Draw(float x, float y);
	void DrawCentered(float cx, float cy, float x, float y, float sx = 1.0f, float sy = 1.0f, float angle = 0.0f, const Color &color = Color());
	void Draw(const MX::Rectangle &destination, const MX::Rectangle &source, const Color &color = Color());


	unsigned Height();
	unsigned Width();

	bool empty();

	bool save(const std::string &path);

	const ci::gl::TextureRef &native_bitmap_handle() const { return _texture; };

	virtual void SetCenter(const glm::vec2& center) { _center = center; }

	static pointer Create(const Surface::pointer& parent, int x, int y, int w, int h)
	{
		auto bit = std::make_shared<Surface>(*parent, x, y, w, h);
		if (bit->empty())
			return nullptr;
		return bit;
	}

	static pointer Create(const Surface::pointer& parent, const MX::Rectangle& rect)
	{
		return Create(parent, (int)rect.x1, (int)rect.y1, (int)rect.width(), (int)rect.height());
	}

	static pointer Create(unsigned width, unsigned height, bool alpha = true)
	{
		auto bit = std::make_shared<Surface>(width, height, alpha);
		if (bit->empty())
			return nullptr;
		return bit;
	}

	static pointer Create(void *data, GLenum dataFormat, unsigned width, unsigned height);
	

protected:
	ci::Area subCoords(int x, int y, int w, int h) const;
	ci::Area subCoords(ci::Area &sub) const { return subCoords(sub.x1, sub.y1, sub.getWidth(), sub.getHeight()); }


	void SetTexture(const ci::gl::TextureRef& texture);
	ci::gl::TextureRef _texture;
	ci::Area           _texCoords;
	MX::glm::vec2 _center;
};

}
}

#endif

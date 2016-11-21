#pragma once
#include "Image.h"

namespace MX{

struct Quad;

namespace Graphic
{

class Display;

class Surface : public Image, public TargetSurface, public disable_copy_constructors, public shared_ptr_init<Surface>
{
public:
	using TexturePointer = std::shared_ptr< class Texture >;

	Surface(const Surface& parent, float x, float y, float w, float h);
	Surface(const Surface& parent, const MX::Rectangle& rect);
	Surface(unsigned width, unsigned height, bool alpha = true);

	Surface(const TexturePointer& texture);
	~Surface();


	void Clear(const Color &color);

	void DrawCentered(const glm::vec2& offset, const glm::vec2& pos, const glm::vec2& scale = { 1.0f, 1.0f }, float angle = 0.0f, const Color &color = Color());
	void Draw(const MX::Rectangle &destination, const MX::Rectangle &source, const Color &color = Color());


	unsigned Height();
	unsigned Width();

	bool empty();

	bool save(const std::string &path);

	const TexturePointer &texture() const { return _texture; };

	virtual void SetCenter(const glm::vec2& center) { _center = center; }

	static pointer Create(const Surface::pointer& parent, float x, float y, float w, float h)
	{
		auto bit = std::make_shared<Surface>(*parent, x, y, w, h);
		if (bit->empty())
			return nullptr;
		return bit;
	}

	static pointer Create(const Surface::pointer& parent, const MX::Rectangle& rect)
	{
		return Create(parent, rect.x1, rect.y1, rect.width(), rect.height());
	}

	static pointer Create(unsigned width, unsigned height, bool alpha = true)
	{
		auto bit = std::make_shared<Surface>(width, height, alpha);
		if (bit->empty())
			return nullptr;
		return bit;
	}

	static pointer Create(void *data, unsigned int dataFormat, unsigned width, unsigned height);
	

protected:
	Rectangle subCoords(float x, float y, float w, float h) const;
	Rectangle subCoords(const Rectangle &sub) const { return subCoords(sub.x1, sub.y1, sub.width(), sub.height()); }


	void SetTexture(const TexturePointer& texture);
	TexturePointer _texture;
	Rectangle      _texCoords;
	glm::vec2      _center;
};

}
}


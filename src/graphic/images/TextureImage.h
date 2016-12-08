#pragma once
#include "Image.h"
#include "Surface.h"

namespace MX{

namespace gl
{
	class Texture;
}
struct Quad;

namespace Graphic
{

class Display;

class TextureImage : public Image, public TargetSurface, public disable_copy_constructors, public shared_ptr_init<TextureImage>
{
public:
	using TexturePointer = std::shared_ptr< gl::Texture >;

	TextureImage(const TextureImage& parent, float x, float y, float w, float h);
	TextureImage(const TextureImage& parent, const MX::Rectangle& rect);
	TextureImage(unsigned width, unsigned height, bool alpha = true);

	TextureImage(const TexturePointer& texture);
	~TextureImage();


	void Clear(const Color &color) override;

	void DrawCentered(const glm::vec2& offset, const glm::vec2& pos, const glm::vec2& scale = { 1.0f, 1.0f }, float angle = 0.0f, const Color &color = Color()) override;
	void DrawArea(const MX::Rectangle &destination, const MX::Rectangle &source, const Color &color = Color()) override;


	unsigned Height() override;
	unsigned Width() override;

	bool empty();

	bool save(const std::string &path);

	const TexturePointer& texture() const override { return _texture; };

	virtual void SetCenter(const glm::vec2& center) { _center = center; }

	static pointer Create(const TextureImage::pointer& parent, float x, float y, float w, float h)
	{
		auto bit = std::make_shared<TextureImage>(*parent, x, y, w, h);
		if (bit->empty())
			return nullptr;
		return bit;
	}

	static pointer Create(const TextureImage::pointer& parent, const MX::Rectangle& rect)
	{
		return Create(parent, rect.x1, rect.y1, rect.width(), rect.height());
	}

	static pointer Create(unsigned width, unsigned height, bool alpha = true)
	{
		auto bit = std::make_shared<TextureImage>(width, height, alpha);
		if (bit->empty())
			return nullptr;
		return bit;
	}

    static pointer Create(const SurfaceRGBA& surface);
    static pointer Create(const SurfaceGrayscale& surface);
	static pointer Create(unsigned width, unsigned height, unsigned int dataFormat, void *data);
	static pointer Create(const std::string& path);

protected:
	glm::vec2 dimensionToUV(const glm::vec2& v);
	glm::vec4 subDimensions(float x, float y, float w, float h) const;
	glm::vec4 subDimensions(const Rectangle &sub) const { return subDimensions(sub.x1, sub.y1, sub.width(), sub.height()); }


	void SetTexture(const TexturePointer& texture);
	TexturePointer _texture;
	Rectangle      _uvCoords;
	glm::vec4      _dimensions;
	glm::vec2      _center;
};

}
}


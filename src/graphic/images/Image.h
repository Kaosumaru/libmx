#pragma once
#include "graphic/Color.h"
#include "utils/Utils.h"
#include "glm/vec2.hpp"
#include "utils/shapes/Rectangle.h"

namespace MX{

struct Quad;

namespace Graphic
{

class TargetContext;

class TargetSurface
{
public:
	friend class TargetContext;

	virtual ~TargetSurface(){}
	
#if WIP	
	virtual const ci::gl::TextureRef &native_bitmap_handle() const = 0;
#endif

	virtual unsigned Height() = 0;
	virtual unsigned Width() = 0;


protected:
	void bindAsTarget();
	void unbindAsTarget();
};

class TargetContext
{
public:
	TargetContext(TargetSurface& target);
	TargetContext(const std::shared_ptr<TargetSurface>& target);
	~TargetContext();
protected:
	static void SetTarget(TargetSurface& target);
	static TargetSurface& target_surface();
	static TargetSurface *_current_target;

	TargetSurface &_target;
};


class ClippingContext
{
public:
	ClippingContext(const Rectangle& rect);
	~ClippingContext();
protected:
	static Rectangle _rect;
};

class Image
{
public:
	virtual ~Image(){}
	virtual void Lock(){};
	virtual void Unlock(){};

	virtual void PutPixel(int x, int y, const Color &color){};
	virtual Color GetPixel(int x, int y){ return Color(); };

	virtual void Clear(const Color &color){};


	//draw
	virtual void Draw(const glm::vec2& pos);
	virtual void DrawTinted(const glm::vec2& pos, const Color &color) ;
	virtual void DrawRotated(const glm::vec2& offset, const glm::vec2& pos, float angle);
	virtual void DrawScaled(const glm::vec2& offset,const glm::vec2& pos, const glm::vec2& scale);
	virtual void DrawCentered(const glm::vec2& offset, const glm::vec2& pos, const glm::vec2& scale = { 1.0f, 1.0f }, float angle = 0.0f, const Color &color = Color()) = 0;

	virtual void DrawTiled(const MX::Rectangle &destination, const Color &color = Color());
	virtual void Draw(const MX::Rectangle &destination, const MX::Rectangle &source, const Color &color = Color()) {};
	virtual void Draw(const MX::Rectangle &destination, const Color &color = Color());

	glm::ivec2 dimensions() { return glm::ivec2{ Height(), Width() }; }
	virtual unsigned Height() = 0;
	virtual unsigned Width() = 0;

	virtual glm::vec2 size() { return glm::vec2((float)Width(), (float)Height()); }

	//TODO
	virtual void AdvanceTime(float seconds) {}
    
    class Hold : public StaticLocker<Hold>
    {
        friend class StaticLocker<Hold>;
        static void Locked();
        static void Unlocked();
    };

	struct Settings
	{
		struct ColorMultiplier {};
		struct ScaleMultiplier {};

		static bool flipX;
		static bool flipY;
		static bool linearSampling;
		static bool premultiplied_alpha;
		static bool mipmap;

		static const Color &modifyColor(const Color &color);
	};

	virtual unsigned int get_opengl_texture() { return 0; }

};

using ImagePtr = std::shared_ptr<Image>;

}
}


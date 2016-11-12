#include "Image.h"

using namespace MX;
using namespace MX::Graphic;


void TargetSurface::bindAsTarget()
{
#ifdef WIP
	if (!_fbo)
	{
		_fbo = std::make_shared<Graphic::RenderToTextureFBO>();
		_fbo->attachTexture(native_bitmap_handle());
	}
	_fbo->bind();
#endif
}

void TargetSurface::unbindAsTarget()
{
#ifdef WIP
	_fbo->unbind();
#endif
}

TargetContext::TargetContext(const std::shared_ptr<TargetSurface>& target) : TargetContext(*target)
{
}

TargetContext::TargetContext(TargetSurface& target) : _target(target_surface())
{
	SetTarget(target);
}

TargetContext::~TargetContext()
{
	SetTarget(_target);
}

void TargetContext::SetTarget(TargetSurface& target)
{
	if (&target == _current_target)
		return;


#ifdef WIP
	Graphic::TextureRenderer::current().Flush();
#endif

	if (_current_target)
		_current_target->unbindAsTarget();


	_current_target = &target;


	if (_current_target)
	{
		_current_target->bindAsTarget();
#ifdef WIP
		Blender::current().Apply(); //TODO why is this needed? Does cinder changes blend modes in pushViewPort?
#endif
	}

}

TargetSurface& TargetContext::target_surface()
{
	return *_current_target;
}

TargetSurface *TargetContext::_current_target = 0;


ClippingContext::ClippingContext(const MX::Rectangle& rect)
{
	assert(_rect.empty());
	_rect = rect;
#ifdef WIP
	Graphic::TextureRenderer::current().Flush();
	glEnable(GL_SCISSOR_TEST);
	auto viewPortHeight = ci::gl::getViewport().second.y;
	glScissor((int)rect.x1, (int)(viewPortHeight - rect.y1 - rect.height()), rect.width(), rect.height());
#endif

}
ClippingContext::~ClippingContext()
{
#ifdef WIP
	Graphic::TextureRenderer::current().Flush();
	glDisable(GL_SCISSOR_TEST);
	_rect.x1 = _rect.x2 = _rect.y1 = _rect.y2 = 0.0f;
#endif
}


MX::Rectangle ClippingContext::_rect;



void Image::Draw(float x, float y)
{
	DrawCentered(0.0f,0.0f,x,y);
}
void Image::DrawTinted(float x, float y, const Color &color)
{
	DrawCentered(0.0f,0.0f,x,y, 1.0f, 1.0f, 0.0f, color);
}
void Image::DrawRotated(float cx, float cy, float x, float y, float angle)
{
	DrawCentered(cx,cy,x,y, 1.0f, 1.0f, angle);
}
void Image::DrawScaled(float cx, float cy, float x, float y, float sx, float sy)
{
	DrawCentered(cx,cy,x,y, sx, sy);
}

void Image::Draw(const MX::Rectangle &destination, const Color &color)
{
	float sx = (float)destination.width()/(float)Width();
	float sy = (float)destination.height()/(float)Height();

	DrawCentered(0.0f,0.0f,(float)destination.x1, (float)destination.y1, sx, sy, 0.0f, color);
}


void Image::DrawTiled(const MX::Rectangle &destination, const Color &color)
{
	auto src = destination;
	src.Shift(-src.x1, -src.y1);
	Draw(destination, src, color);
}





void Image::Hold::Locked()
{
	//currently, drawing is implicitly locked
}
void Image::Hold::Unlocked()
{

}

bool Image::Settings::flipX = false;
bool Image::Settings::flipY = false;
bool Image::Settings::linearSampling = true;
bool Image::Settings::premultiplied_alpha = true;
bool Image::Settings::mipmap = false;

const Color &Image::Settings::modifyColor(const Color &color)
{
	using ScopeMultiplier = Context<Color, Image::Settings::ColorMultiplier>;

	if (ScopeMultiplier::isCurrent())
	{
		static Color scolor;
		scolor = color * ScopeMultiplier::current();
		return scolor;
	}

	return color;
}
const float& Image::Settings::modifyScaleX(const float & scaleX)
{
	typedef Context<glm::vec2, Image::Settings::ScaleMultiplier> ScopeMultiplier;

	if (ScopeMultiplier::isCurrent())
	{
		static float sscale;
		sscale = scaleX * ScopeMultiplier::current().x;
		return sscale;
	}

	return scaleX;
}
const float& Image::Settings::modifyScaleY(const float & scaleY)
{
	typedef Context<glm::vec2, Image::Settings::ScaleMultiplier> ScopeMultiplier;

	if (ScopeMultiplier::isCurrent())
	{
		static float sscale;
		sscale = scaleY * ScopeMultiplier::current().y;
		return sscale;
	}

	return scaleY;
}
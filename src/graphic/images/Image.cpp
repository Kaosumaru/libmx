#include "Image.h"
#include "graphic/opengl/Framebuffer.h"

using namespace MX;
using namespace MX::Graphic;


void TargetSurface::bindAsTarget()
{
	if (!_fbo)
	{
		_fbo = std::make_shared<gl::Framebuffer>();
		_fbo->Bind();
		_fbo->AttachTexture(*texture());
		return;
	}
	_fbo->Bind();
#if WIP
	ci::gl::pushViewport({ 0,0 }, _textureArea.getSize());
	ci::gl::pushMatrices();
	ci::gl::setMatricesWindow(_textureArea.getWidth(),_textureArea.getHeight(), true);
#endif
}

void TargetSurface::unbindAsTarget()
{
	_fbo->Unbind();
#if WIP
	ci::gl::popMatrices();
	ci::gl::popViewport();;
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



void Image::Draw(const glm::vec2& pos)
{
	DrawCentered({}, pos);
}
void Image::DrawTinted(const glm::vec2& pos, const Color &color)
{
	DrawCentered({}, pos, { 1.0f, 1.0f }, 0.0f, color);
}
void Image::DrawRotated(const glm::vec2& offset, const glm::vec2& pos, float angle)
{
	DrawCentered(offset, pos, { 1.0f, 1.0f }, angle);
}
void Image::DrawScaled(const glm::vec2& offset, const glm::vec2& pos, const glm::vec2& scale)
{
	DrawCentered(offset, pos, scale);
}

void Image::Draw(const MX::Rectangle &destination, const Color &color)
{
	float sx = (float)destination.width()/(float)Width();
	float sy = (float)destination.height()/(float)Height();

	DrawCentered({}, { (float)destination.x1, (float)destination.y1 }, { sx, sy }, 0.0f, color);
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

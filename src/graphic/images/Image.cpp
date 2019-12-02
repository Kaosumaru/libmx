#include "Image.h"
#include "glm/gtc/matrix_transform.hpp"
#include "graphic/Blender.h"
#include "graphic/opengl/Framebuffer.h"
#include "graphic/renderer/MVP.h"
#include "graphic/renderer/TextureRenderer.h"
#include "graphic/renderer/Viewport.h"
#include <iostream>

using namespace MX;
using namespace MX::Graphic;

void TargetSurface::bindAsTarget()
{
    if (!_fbo)
    {
        _fbo = std::make_shared<gl::Framebuffer>();
        _fbo->Bind();
        _fbo->AttachTexture(*texture());
    }
    else
        _fbo->Bind();

    Viewport::Push(texture()->width(), texture()->height());
    MVP::Push();
    glm::mat4x4 projection = glm::orthoLH(0.0f, (float)texture()->width(), 0.0f, (float)texture()->height(), -100.0f, 100.0f);
    MVP::SetProjection(projection);
}

void TargetSurface::unbindAsTarget()
{
    _fbo->Unbind();
    MVP::Pop();
    Viewport::Pop();
}

TargetContext::TargetContext(const std::shared_ptr<TargetSurface>& target)
    : TargetContext(*target)
{
}

TargetContext::TargetContext(TargetSurface& target)
    : _oldTarget(_current_target)
{
    SetTarget(&target);
}

TargetContext::~TargetContext()
{
    SetTarget(_oldTarget);
}

void TargetContext::SetTarget(TargetSurface* target)
{
    if (target == _current_target)
        return;

    Graphic::TextureRenderer::current().Flush();

    if (_current_target)
        _current_target->unbindAsTarget();

    _current_target = target;

    if (_current_target)
        _current_target->bindAsTarget();
}

TargetSurface& TargetContext::target_surface()
{
    return *_current_target;
}

TargetSurface* TargetContext::_current_target = 0;

ClippingContext::ClippingContext(const MX::Rectangle& rect)
{
    assert(_rect.empty());
    _rect = rect;
    Graphic::TextureRenderer::current().Flush();
    glEnable(GL_SCISSOR_TEST);
    auto viewPortHeight = Viewport::current().height();
    glScissor((int)rect.x1, (int)(viewPortHeight - rect.y1 - rect.height()), (int)rect.width(), (int)rect.height());
}

ClippingContext::~ClippingContext()
{
    Graphic::TextureRenderer::current().Flush();
    glDisable(GL_SCISSOR_TEST);
    _rect.x1 = _rect.x2 = _rect.y1 = _rect.y2 = 0.0f;
}

MX::Rectangle ClippingContext::_rect;

void Image::Draw(const glm::vec2& pos)
{
    DrawCentered({}, pos);
}
void Image::DrawTinted(const glm::vec2& pos, const Color& color)
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

void Image::DrawArea(const MX::Rectangle& destination, const Color& color)
{
    float sx = (float)destination.width() / (float)Width();
    float sy = (float)destination.height() / (float)Height();

    DrawCentered({}, { (float)destination.x1, (float)destination.y1 }, { sx, sy }, 0.0f, color);
}

void Image::DrawTiled(const MX::Rectangle& destination, const Color& color)
{
    auto src = destination;
    src.Shift(-src.x1, -src.y1);
    DrawArea(destination, src, color);
}

bool Image::Settings::flipX = false;
bool Image::Settings::flipY = false;
bool Image::Settings::linearSampling = true;
bool Image::Settings::premultiplied_alpha = true;
bool Image::Settings::mipmap = false;

const Color& Image::Settings::modifyColor(const Color& color)
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

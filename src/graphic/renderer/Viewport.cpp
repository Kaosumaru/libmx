#include "Viewport.h"
#include "graphic/renderer/TextureRenderer.h"

using namespace MX;

void Viewport::Push()
{
    Graphic::TextureRenderer::current().Flush();
    auto c = get().current();
    get()._stack.push_back(c);
}

void Viewport::Push(int w, int h)
{
    Graphic::TextureRenderer::current().Flush();
    get()._stack.push_back(Rectangle { 0.0f, 0.0f, (float)w, (float)h });
    UseCurrent();
}

void Viewport::Pop()
{
    Graphic::TextureRenderer::current().Flush();
    get()._stack.pop_back();
    UseCurrent();
}
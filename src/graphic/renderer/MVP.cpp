#include "MVP.h"
#include "glm/gtc/matrix_transform.hpp"
#include "graphic/renderer/TextureRenderer.h"

using namespace MX;

void MVP::Push()
{
    Graphic::TextureRenderer::current().Flush();
    auto c = get().current();
    get()._stack.push_back(c);
}

void MVP::Pop()
{
    Graphic::TextureRenderer::current().Flush();
    get()._stack.pop_back();
}

void MVP::translate(const glm::vec2& p)
{
    auto& m = get().current()._model;
    m = glm::translate(m, { p, 0.0f });
    get().current().UpdateMVP();
}

void MVP::rotateZoom(const glm::vec2& center, const glm::vec2& scale, float angle)
{
    auto& m = get().current()._model;
    m = glm::translate(m, { center, 0.0f });
    m = glm::rotate(m, angle, { 0.0f, 0.0f, 1.0f });
    m = glm::scale(m, { scale, 1.0f });
    m = glm::translate(m, { -center, 0.0f });
    get().current().UpdateMVP();
}
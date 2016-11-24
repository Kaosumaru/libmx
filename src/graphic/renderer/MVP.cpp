#include "MVP.h"
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
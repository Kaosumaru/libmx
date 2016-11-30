#include "Blender.h"
#include "graphic/renderer/TextureRenderer.h"
#include "OPenGL.h"

using namespace MX;
using namespace Graphic;

Blender::Blender(int op, int src, int dst)
{
    _op = op;
    _src = src;
    _dst = dst;
	_alpha_op = GL_FUNC_ADD;
	_alpha_src = GL_ZERO;
	_alpha_dst = GL_ONE;
}

Blender::Blender(int op, int src, int dst, int alpha_op, int alpha_src, int alpha_dst)
{
    _op = op;
    _src = src;
    _dst = dst;
	_alpha_op = alpha_op;
	_alpha_src = alpha_src;
	_alpha_dst = alpha_dst;
}

Blender::~Blender()
{
    
}

bool Blender::operator == (const Blender &other) const
{
	if (_op == other._op &&
		_src == other._src &&
		_dst == other._dst &&
		_alpha_op == other._alpha_op &&
		_alpha_src == other._alpha_src &&
		_alpha_dst == other._alpha_dst)
		return true;
	return false;
}

Blender& Blender::defaultNormal()
{
	static Graphic::Blender blender(GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD, GL_ZERO, GL_ONE);
	return blender;
}
Blender& Blender::defaultAdditive()
{
	static Graphic::Blender blender(GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE, GL_FUNC_ADD, GL_ZERO, GL_ONE);
	return blender;
}
Blender& Blender::defaultCopy()
{
	static Graphic::Blender blender(GL_FUNC_ADD, GL_ONE, GL_ZERO, GL_FUNC_ADD, GL_ONE, GL_ZERO);
	return blender;
}

Blender& Blender::defaultPremultiplied()
{
	static Graphic::Blender blender(GL_FUNC_ADD, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD, GL_ZERO, GL_ONE);
	return blender;
}

void Blender::Apply()
{
	Graphic::TextureRenderer::current().Flush();



	if (_alpha_op == -1)
	{
		glBlendFunc(_src, _dst);
		glBlendEquation(_op);
	}

	else
	{
		glBlendFuncSeparate(_src, _dst, _alpha_src, _alpha_dst);
		glBlendEquationSeparate(_op, _alpha_op);

	}
}

void Blender::CurrentWasSet(Blender* target, Blender* old_target)
{
	if (target == old_target)
		return;


	if (target == nullptr)
	{
		Graphic::TextureRenderer::current().Flush();
		return;
	}
	target->Apply();
}

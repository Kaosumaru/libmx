#pragma once
#include "graphic/OpenGL.h"
#include "graphic/Color.h"

namespace MX
{
namespace gl
{
	inline void Clear(const MX::Color& c)
	{
		glClearColor(c.r(), c.g(), c.b(), c.a());
	}

}
}

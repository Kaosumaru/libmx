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
		glClear(GL_COLOR_BUFFER_BIT);
	}

	inline unsigned long UpperPowerOfTwo(unsigned long v)
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}


}
}

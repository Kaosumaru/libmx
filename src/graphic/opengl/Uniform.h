#pragma once
#include "graphic/OpenGL.h"
#include "glm/glm.hpp"

namespace MX
{
namespace gl
{
	void Uniform(GLuint location, const glm::mat4x4& m, bool transpose = false)
	{
		glUniformMatrix4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, (float*)&m);
	}

	void Uniform(GLuint location, const glm::vec4& v, bool transpose = false)
	{
		glUniform4fv(location, 1, (float*)&v);
	}

}
}

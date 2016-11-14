#pragma once
#include "graphic/OpenGL.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace MX
{
namespace gl
{
	inline void Uniform(GLuint location, const glm::mat4x4& m, bool transpose = false)
	{
		glUniformMatrix4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(m));
	}

	inline void Uniform(GLuint location, const glm::vec4& v, bool transpose = false)
	{
		glUniform4fv(location, 1, glm::value_ptr(v));
	}

}
}

#pragma once
#include "graphic/OpenGL.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace MX::gl
{
	inline void SetUniform(GLuint location, const glm::mat4x4& m, bool transpose = false)
	{
		glUniformMatrix4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(m));
	}

	inline void SetUniform(GLuint location, const glm::vec4& v)
	{
		glUniform4fv(location, 1, glm::value_ptr(v));
	}

	inline void SetUniform(GLuint location, const glm::vec3& v)
	{
		glUniform3fv(location, 1, glm::value_ptr(v));
	}

	inline void SetUniform(GLuint location, const glm::vec2& v)
	{
		glUniform2fv(location, 1, glm::value_ptr(v));
	}

	inline void SetUniform(GLuint location, float v)
	{
		glUniform1f(location, v);
	}

	inline void SetUniform(GLuint location, const glm::ivec4& v)
	{
		glUniform4iv(location, 1, glm::value_ptr(v));
	}

	inline void SetUniform(GLuint location, const glm::ivec3& v)
	{
		glUniform3iv(location, 1, glm::value_ptr(v));
	}

	inline void SetUniform(GLuint location, const glm::ivec2& v)
	{
		glUniform2iv(location, 1, glm::value_ptr(v));
	}

	inline void SetUniform(GLuint location, int v)
	{
		glUniform1i(location, v);
	}
}

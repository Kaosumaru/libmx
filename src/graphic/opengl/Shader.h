#pragma once
#include "graphic/OpenGL.h"
#include <string>
	
namespace MX
{
namespace gl
{
	inline void ShaderDeleter(GLuint obj)
	{
		glDeleteShader(obj);
	}

	class Shader : public GLObject<ShaderDeleter>
	{
	public:
		enum class Type
		{
			Vertex = GL_VERTEX_SHADER,
			Fragment = GL_FRAGMENT_SHADER
		};

		bool Compile(const std::string& source, Type type)
		{
			return Compile(source.c_str(), type);
		}

		bool Compile(const char* source, Type type)
		{
			Set(glCreateShader((GLenum)type));
			const GLchar* shaderSource[] = { source };
			glShaderSource(*this, 1, shaderSource, nullptr);
			glCompileShader(*this);
			GLint vShaderCompiled = GL_FALSE;
			glGetShaderiv(*this, GL_COMPILE_STATUS, &vShaderCompiled);
			return vShaderCompiled != GL_FALSE;
		}

		std::string infoLog() const
		{
			if (!*this)
				return "";
			int infoLogLength = 0;
			int maxLength = infoLogLength;
			glGetShaderiv(*this, GL_INFO_LOG_LENGTH, &maxLength);

			std::string text;
			text.resize(maxLength);
			glGetShaderInfoLog(*this, maxLength, &infoLogLength, &(text[0]));
			return text;
		}

	protected:
	};
}
}

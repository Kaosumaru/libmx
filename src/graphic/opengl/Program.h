#pragma once
#include <memory>
#include "graphic/OpenGL.h"
#include "Shader.h"

namespace MX
{
namespace gl
{
	inline void ProgramDeleter(GLuint obj)
	{
		glDeleteProgram(obj);
	}

	class Program : public GLObject<ProgramDeleter>
	{
	public:
        using pointer = std::shared_ptr<Program>;
		Program()
		{
			
		}

		void AttachShader(const Shader& shader)
		{
			if (!*this)
				Set(glCreateProgram());
			glAttachShader(*this, shader.Get());
		}

		bool Link()
		{
			glLinkProgram(*this);
			GLint programSuccess = GL_TRUE;
			glGetProgramiv(*this, GL_LINK_STATUS, &programSuccess);
			return programSuccess == GL_TRUE;
		}

		void Use()
		{
			glUseProgram(*this);
		}

		std::string infoLog() const
		{
			if (!*this)
				return "";
			int infoLogLength = 0;
			int maxLength = infoLogLength;
			glGetProgramiv(*this, GL_INFO_LOG_LENGTH, &maxLength);

			std::string text;
			text.resize(maxLength);
			glGetProgramInfoLog(*this, maxLength, &infoLogLength, &(text[0]));
			return text;
		}


		GLuint GetAttribLocation(const std::string& name) { return GetAttribLocation(name.c_str()); }
		GLuint GetAttribLocation(const char* name)
		{
			return glGetAttribLocation(*this, name);
		}

		GLuint GetUniformLocation(const std::string& name) { return GetAttribLocation(name.c_str()); }
		GLuint GetUniformLocation(const char* name)
		{
			return glGetUniformLocation(*this, name);
		}
	};

	Program::pointer createProgramFromFiles(const std::string& vertexShader, const std::string& fragmentShader, std::string& errorLog);
	Program::pointer createProgram(const std::string& vertexShader, const std::string& fragmentShader, std::string& errorLog);
}
}

#pragma once
#include "graphic/OpenGL.h"
#include "Shader.h"

namespace MX
{
namespace gl
{
	inline void BufferDeleter(GLuint obj)
	{
		glDeleteProgram(obj);
	}

	class Buffer : public GLObject<BufferDeleter>
	{
	public:
		enum class Type
		{
			Array = GL_ARRAY_BUFFER
		};

		Buffer()
		{

		}

		void Create(Type type)
		{
			_type = type;
			glGenBuffers(1, &_object);
		}

		void Bind()
		{
			glBindBuffer((GLuint)_type, _object);
		}

		template<typename T, int size>
		void Data(T(&data)[size], GLuint usage)
		{
			glBufferData((GLuint)_type, size * sizeof(T), data, usage);
		}

		void Data(void *data, GLuint size, GLuint usage)
		{
			glBufferData((GLuint)_type, size, data, usage);
		}

	protected:
		Type _type;
	};
}
}

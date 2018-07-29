#ifndef TEXTURE
#define TEXTURE
#include "utils/Utils.h"
#include "utils/Vector2.h"
#include "graphic/OpenGL.h"

namespace MX 
{
namespace gl
{

	class Texture : public shared_ptr_init<Texture>
	{
	public:
		struct LoadInfo
		{
			bool generateMipMap = false;
			int minFilter = GL_LINEAR;
			int magFilter = GL_LINEAR;

			void setupMipMap();
		};

		Texture(unsigned w, unsigned h, GLint target, GLint format, const GLvoid *data);
		Texture(const glm::ivec2& size, bool alpha = true);
		Texture(const std::string& path);
		~Texture();


		static pointer Create(const glm::vec2& size, bool alpha = true);
		static pointer Create(const std::string& str);

		bool empty() const { return getId() == -1; }
		GLuint getId() const { return _textureID; }
		glm::vec2 size() { return glm::vec2{ _width, _height }; }

		auto width() { return _width; }
		auto height() { return _height; }

		void SetWrap(GLint wrap_s, GLint wrap_t);

		void bind(int unit = 0);
		void unbind(int unit = 0);

		glm::vec2 pixelToUV(const glm::vec2& pixel);

	protected:
		void constructTexture(unsigned w, unsigned h, GLint target, GLint format, const GLvoid *data);

		unsigned _width = 0, _height = 0;
		GLuint _textureID = -1;

	};




}
}


#endif

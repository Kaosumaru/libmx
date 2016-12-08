#include <memory>
#include "Texture.h"
#include "deps/lodepng.h"

using namespace MX;
using namespace MX::gl;

Texture::Texture(unsigned w, unsigned h, GLint target, GLint format, const GLvoid *data)
{
	constructTexture(w, h, target, format, data);
}

Texture::Texture(const glm::ivec2& size, bool alpha)
{
	auto format = alpha ? GL_RGBA : GL_RGB;
	constructTexture(size.x, size.y, format, format, nullptr);
}

Texture::Texture(const std::string & path)
{
	std::vector<unsigned char> out;
	unsigned w = 0, h = 0;
	auto ret = lodepng::decode(out, w, h, path);
	if (ret == 0)
		constructTexture(w, h, GL_RGBA, GL_RGBA, out.data());
}

Texture::~Texture()
{
	if (_textureID != -1)
		glDeleteTextures(1, &_textureID);

}

void Texture::bind(int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, _textureID);
}
void Texture::unbind(int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::constructTexture(unsigned w, unsigned h, GLint target, GLint format, const GLvoid *data)
{
	_width = w;
	_height = h;
	glGenTextures(1, &_textureID);
	glBindTexture(GL_TEXTURE_2D, _textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		target,
		(GLsizei)_width,
		(GLsizei)_height,
		0,
		format,
		GL_UNSIGNED_BYTE,
		data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

glm::vec2 Texture::pixelToUV(const glm::vec2& pixel)
{
	return { pixel.x / _width,  pixel.y / _height };
}

void Texture::SetWrap(GLint wrap_s, GLint wrap_t)
{
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
	unbind();
}

Texture::pointer Texture::Create(const glm::vec2& size, bool alpha)
{
	return std::make_shared<Texture>(size, alpha);
}

Texture::pointer Texture::Create(const std::string & str)
{
	auto tex = std::make_shared<Texture>(str);
	if (tex->empty())
		return nullptr;

	return tex;
}

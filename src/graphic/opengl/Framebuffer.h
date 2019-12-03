#pragma once
#include "Texture.h"
#include "graphic/OpenGL.h"

namespace MX
{
namespace gl
{
    inline void FramebufferDeleter(GLuint obj)
    {
        glDeleteFramebuffers(1, &obj);
    }

    class Framebuffer : public GLObject<FramebufferDeleter>
    {
    public:
        Framebuffer()
        {
            glGenFramebuffers(1, &_object);
        }

        void Create()
        {
            glGenBuffers(1, &_object);
        }

        void Bind()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, _object);
        }

        static void Unbind()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        static void AttachTexture(const Texture& texture)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.getId(), 0);
#if !defined(MX_GLES)
            GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
#endif
        }
    };
}
}

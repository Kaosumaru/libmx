#pragma once
#include <GL/glew.h>
#include "SDL_opengl.h"

namespace MX
{
template <void (*deleter)(GLuint)>
struct GLObject
{
    GLObject() { }
    GLObject(GLuint object)
        : _object(object)
    {
    }
    GLObject(const GLObject&) = delete;
    GLObject(GLObject&& other)
    {
        _object = other._object;
        other.Orphan();
    }

    ~GLObject()
    {
        Delete();
    }

    operator bool() const
    {
        return _object != 0;
    }

    operator GLuint() const
    {
        return _object;
    }

    GLObject& operator=(GLuint obj)
    {
        Set(obj);
        return *this;
    }

    GLObject& operator=(GLObject&& obj)
    {
        Set(obj._object);
        obj.Orphan();
        return *this;
    }

    GLuint Get() const
    {
        return _object;
    }

    void Orphan()
    {
        _object = 0;
    }

    void Delete()
    {
        if (_object)
            deleter(_object);
    }

    void Set(GLuint obj)
    {
        Delete();
        _object = obj;
    }

protected:
    GLuint _object = 0;
};
}

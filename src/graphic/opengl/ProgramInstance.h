#pragma once
#include "graphic/opengl/Program.h"
#include "graphic/opengl/Uniform.h"
#include <vector>

namespace MX::gl
{
class ProgramInstance;

class UniformBase
{
public:
    UniformBase(ProgramInstance* parent, const char* name);

    void Apply()
    {
        if (_location == -1)
        {
            _location = program()->GetUniformLocation(_name);
        }
        onApply();
    }

protected:
    void MarkParentAsDirty();
    const Program::pointer& program();
    virtual void onApply() {}

    const char* _name;
    ProgramInstance* _parent;
    GLuint _location = -1;
};

class ProgramInstance
{
public:
    friend class UniformBase;
    ProgramInstance() {}
    ProgramInstance(const Program::pointer& program)
        : _program(program)
    {
    }

    ProgramInstance(const char* fragmentPath, const char* vertexPath = nullptr);

    const Program::pointer& program() { return _program; }

    void Use();

protected:
    bool _dirty = false;
    std::vector<UniformBase*> _uniforms;
    Program::pointer _program;

    static ProgramInstance* s_current;
};

template <typename T>
class Uniform : public UniformBase
{
public:
    using UniformBase::UniformBase;
    Uniform(ProgramInstance* parent, const char* name, const T& v)
        : UniformBase(parent, name)
    {
        _value = v;
    }

    Uniform& operator=(const T& v)
    {
        _value = v;
        MarkParentAsDirty();
        return *this;
    }

    const T& value()
    {
        return _value;
    }

protected:
    void onApply() override
    {
        SetUniform(_location, _value);
    }

    T _value {};
};
}

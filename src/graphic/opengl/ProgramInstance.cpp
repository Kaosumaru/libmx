#include "ProgramInstance.h"
#include "graphic/renderer/DefaultRenderers.h"

using namespace MX::gl;

UniformBase::UniformBase(ProgramInstance* parent, const char* name)
    : _name(name)
    , _parent(parent)
{
    _parent->_uniforms.push_back(this);
}

void UniformBase::MarkAsDirty()
{
    _dirty = true;
    _parent->_hasDirtyUniforms = true;
}

const Program::pointer& UniformBase::program()
{
    return _parent->program();
}

ProgramInstance::ProgramInstance(const char* fragmentPath, const char* vertexPath)
{
    _program = Graphic::Renderers::get().createProgram(fragmentPath, vertexPath);
}

void ProgramInstance::Use()
{
    program()->Use();
    s_current = this;

    bool shaderOwnerChanged = false;
    if (program()->ownerID() != (std::uintptr_t)this)
    {
        shaderOwnerChanged = true;
        program()->SetOwnerID((std::uintptr_t)this);
    }
    if (!shaderOwnerChanged && !_hasDirtyUniforms)
        return;
    for (auto& uniform : _uniforms)
        uniform->Apply(shaderOwnerChanged);
    _hasDirtyUniforms = false;
}

ProgramInstance* ProgramInstance::s_current = nullptr;
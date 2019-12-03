
#include "ProgramInstance.h"
#include "graphic/renderer/DefaultRenderers.h"

using namespace MX::gl;

UniformBase::UniformBase(ProgramInstance* parent, const char* name)
    : _name(name)
    , _parent(parent)
{
    _parent->_uniforms.push_back(this);
}

void UniformBase::MarkParentAsDirty()
{
    _parent->_dirty = true;
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
    if (program()->owner() != (std::uintptr_t)this)
    {
        _dirty = true;
        program()->SetOwner((std::uintptr_t)this);
	}
    if (!_dirty)
        return;
    for (auto& uniform : _uniforms)
        uniform->Apply();
    _dirty = false;
}

ProgramInstance* ProgramInstance::s_current = nullptr;
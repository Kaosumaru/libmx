
#include "ProgramInstance.h"

using namespace MX::gl;

UniformBase::UniformBase(const char* name, ProgramInstance* parent)
    : _name(name)
    , _parent(parent)
{
}

const Program::pointer& UniformBase::program()
{
    return _parent->program();
}

void ProgramInstance::Use()
{
    program()->Use();
    s_current = this;
}

ProgramInstance* ProgramInstance::s_current = nullptr;
#pragma once
#include "graphic/opengl/Program.h"


namespace MX
{
namespace gl
{
    class UniformBase
    {
    public:
        virtual void Apply() {}
    };
    class ProgramInstance
    {
    protected:
        Program::pointer _program;
    };

}
}

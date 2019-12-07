#pragma once
#include "graphic/opengl/ProgramInstance.h"

#include <vector>

namespace MX::gl
{

class TimedProgramInstance : public ProgramInstance
{
public:
    using ProgramInstance::ProgramInstance;

    MX::gl::Uniform<float> time = { this, "time" };
};

}

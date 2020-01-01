#pragma once
#include "graphic/opengl/TimedProgramInstance.h"
#include "script/ScriptObject.h"

namespace MX::gl
{

class ScriptableProgramInstance : public ProgramInstance, public MX::ScriptObject
{
public:
    ScriptableProgramInstance(MX::LScriptObject& script);
};

class ScriptableTimedProgramInstance : public ScriptableProgramInstance
{
public:
    using ScriptableProgramInstance::ScriptableProgramInstance;

    MX::gl::Uniform<float> time = { this, "time" };
};

class ScriptableProgramInit
{
public:
    static void Init();
};

}

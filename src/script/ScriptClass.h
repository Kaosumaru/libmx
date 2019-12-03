#pragma once
#include "ScriptObject.h"
#include "utils/Utils.h"
#include <string>

namespace MX
{

namespace Scriptable
{
    class Value;
}

class ScriptClass : public ScriptObjectString
{
public:
    virtual ~ScriptClass();

    bool Parse(const std::wstring& className, const std::string& instancePath);

protected:
    virtual bool onParse() { return true; }

    std::wstring _class;
};

}

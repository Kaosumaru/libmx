#pragma once
#include <string>
#include "utils/Utils.h"
#include "ScriptObject.h"

namespace MX{


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
	virtual bool onParse(){return true; }

	std::wstring _class;
};


}

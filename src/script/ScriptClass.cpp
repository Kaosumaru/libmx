#include "ScriptClass.h"
#include "Script.h"

using namespace MX;



ScriptClass::~ScriptClass()
{
}




bool ScriptClass::Parse(const std::wstring& className, const std::string& instancePath)
{
	_class = className;
	SetObjectName(instancePath);
	return onParse();
}
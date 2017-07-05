#include "ScriptObject.h"
#include "Script.h"
#include <string>
#include <codecvt>
#include <locale>
using namespace MX;


ScriptString::ScriptString(const char *label) : _label(label)
{
}

ScriptString::ScriptString(const std::string &label) : _data(label), _label(_data.c_str())
{
}

ScriptString::ScriptString(ScriptString &&Script)
{
}

ScriptString::ScriptString(std::string &&label) : _data(std::move(label)), _label(_data.c_str())
{

}

ScriptString::operator const std::wstring () const
{
	return text();
}

ScriptString::operator const std::string() const
{
	return atext();
}

std::wstring ScriptString::text() const
{
	return MX::loc(_label);
}

std::string ScriptString::atext() const
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
	std::string smatch = convert.to_bytes(MX::loc(_label));
	return smatch;
}



std::wostream & operator<< (std::wostream &out, ScriptString const &t)
{
	out << t.operator const std::wstring();
	return out;
}

ScriptObject::~ScriptObject()
{
}

const Scriptable::Value::pointer& ScriptObject::property_object(const std::string &name)
{
    static Scriptable::Value::pointer null;
	auto& obj = object();
	if (obj.empty()) return null;
	return Script::propertyOrNull(obj, name);
}

const std::string& ScriptObject::object()
{
	static thread_local std::string p;
	return p;
}

std::wstring ScriptObject::name()
{
	auto obj = property_object("Name");
	if (obj)
		return obj->text();
	return L"Misc.UnsetValue";
}
std::wstring ScriptObject::description()
{
	auto obj = property_object("Description");
	if (obj)
		return obj->text();
	return L"Misc.UnsetValue";
}

#ifdef WIPSERIALIZE
ScriptSettings::ScriptSettings(const std::string& objectName, const std::string& prefix) : ScriptObjectString(prefix + objectName) 
{

}

ScriptSettings::~ScriptSettings()
{
	if (_started)
		MX::Scriptable::VariableSerializer::current().CommitTransaction();
}

Scriptable::Variable ScriptSettings::variable(const std::string &name)
{
	if (!_started)
	{
		_started = true;
		MX::Scriptable::VariableSerializer::current().BeginTransaction();
	}
	return ScriptObjectString::variable(name);
}
#endif
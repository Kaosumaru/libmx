#include "ScriptableValue.h"
#include <memory>
#include <regex>
#include <iterator>
#include <sstream>
#include <iostream>

using namespace MX;

std::string Scriptable::Detail::ValueMember::valueString()
{
	return "";
}
std::wstring Scriptable::Detail::ValueMember::valueWstring()
{
	return L"";
}


const Scriptable::Detail::ValueMember::Vector& Scriptable::Detail::ValueMember::array() const
{
	static Vector arr;
	return arr;
}

const Scriptable::Detail::ValueMember::Map& Scriptable::Detail::ValueMember::map() const
{
	static Map arr;
	return arr;
}

class StringValue : public Scriptable::Detail::ValueMember
{
public:
	StringValue(Scriptable::Value *parentValue, const std::wstring &value) : _parentValue(parentValue), _value(value) { _parsed = false; }

	bool isTrue() { Parse(); return true; }
	float valueFloat() { Parse(); return std::stof(_value); }
	std::string valueString() 
	{
		Parse();
        static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
		return convert.to_bytes(_value); 
	}
	std::wstring valueWstring() { Parse(); return _value; }
protected:
	void Parse()
	{
#ifdef WIPSCRIPT
		_value = Script::parseString(_parentValue->path(), _value);
		_parsed = true;
#endif
	}

	std::wstring _value;
	Scriptable::Value *_parentValue;
	bool _parsed;
};

class BoolValue : public Scriptable::Detail::ValueMember
{
public:
	BoolValue(bool value) : _value(value) {}

	bool isTrue() { return _value; }

	float valueFloat() { return _value ? 1.0f : 0.0f; }
	std::string valueString() { return _value ? "Yes" : "No" ;}
	std::wstring valueWstring() { return _value ? L"Yes" : L"No" ;}
protected:
	bool _value;
};



class NumericalValueMember : public Scriptable::Detail::ValueMember
{
public:
	std::string valueString() override
	{
		return std::to_string(valueFloat());
	}
	std::wstring valueWstring() override
	{
		return std::to_wstring(valueFloat());
	}
};

class FloatValue : public NumericalValueMember
{
public:
	FloatValue(float value) : _value(value) {}

	float valueFloat() { return _value; }
protected:
	float _value;
};

class PercentValue : public FloatValue
{
public:
	PercentValue(float value) : FloatValue(value) {}

	Scriptable::ValueType type() override { return Scriptable::ValueType::Percent; }
};





class FunctorValue : public NumericalValueMember
{
public:
	FunctorValue(const std::function<float()> &number_functor) : _number_functor(number_functor) {}

	float valueFloat() { return _number_functor(); }

	bool isConstant() const override { return false; }
protected:
	std::function<float()> _number_functor;
};
 
#ifdef WIPSCRIPT
class RPNValue : public NumericalValueMember
{
public:
	RPNValue(Scriptable::Value *parentValue, std::string &&txt) : _parentValue(parentValue), _text(std::move(txt))
	{
		if (isOnMainThread)
			finalizeOnMainThread();
	}

	~RPNValue()
	{
#ifdef _DEBUG
		_function.Release();
#endif
	}

	void finalizeOnMainThread() override
	{
		auto guard = Context<std::string, MX::KeyContext>::Lock(const_cast<std::string &>(_parentValue->fullPath()));

#ifdef _DEBUG
		_function = MX::RPNParser::Default().Compile(_text);
#else
		_value = MX::RPNParser::Default().Parse(_text);
#endif

		_text.clear();
	}

	float valueFloat() 
	{ 
#ifdef _DEBUG
		return _function();
#else
		return _value->value();
#endif
	}

	bool isConstant() const override 
	{ 
#ifdef _DEBUG
		return _function.constant();
#else
		return _value->constant();
#endif
	}

	std::string valueString() override
	{
#ifdef _DEBUG
		auto &token = _function.token();
#else
		auto &token = _value;
#endif
		return token->stringValue();
	}
	std::wstring valueWstring() override
	{
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
		return convert.from_bytes(valueString());
	}

protected:
	Scriptable::Value* _parentValue;
	std::string _text;

#ifdef _DEBUG
	RPN::Parser::CompiledFunction _function;
#else
	RPN::TokenPtr _value;
#endif
	
};
#endif

class ArrayValue : public Scriptable::Detail::ValueMember
{
public:
	ArrayValue(const std::vector<Scriptable::Value::pointer>& members) : _members(members){	}

	size_t size() const override { return _members.size(); }
	const Vector& array() const override {return _members; }
	std::shared_ptr<Scriptable::Value> at(size_t index) override { return _members[index]; }

	std::string valueString() override
	{
		std::stringstream ss;
		for (auto &member : _members)
			ss << member->atext() << std::endl;
		return ss.str();
	}

	std::wstring valueWstring() override
	{
		std::wstringstream ss;
		for (auto &member : _members)
			ss << member->text() << std::endl;
		return ss.str();
	}
protected:
	std::vector<Scriptable::Value::pointer> _members;
};


class ObjectValue : public Scriptable::Detail::ValueMember
{
public:
	ObjectValue(const std::string& fullPath, const Map& map) : _fullPath(fullPath), _members(map) {}
	ObjectValue(const std::string& fullPath, const std::shared_ptr<ScriptObject>& object) : _fullPath(fullPath), _object(object) {}


	std::shared_ptr<ScriptObject> object() override { return _object; }

#ifdef WIPSCRIPT
	std::shared_ptr<ScriptObject> createObject() override { return _object ? _object : ScriptClassParser::CreateObjectFromTemplate(_fullPath); }
	ScriptObject* createRawObject() override { return ScriptClassParser::CreateRawObjectFromTemplate(_fullPath); }
#endif

	std::string valueString() override
	{
		std::stringstream ss;
		ss << "(" << _fullPath << ") ";
		for (auto &member : _members)
			ss << member.first << ": " << member.second->atext() << std::endl;
		return ss.str();
	}

	std::wstring valueWstring() override
	{
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
		std::wstringstream ss;
		ss << "(" << convert.from_bytes(_fullPath) << ") ";
		for (auto &member : _members)
			ss << convert.from_bytes(member.first) << L": " << member.second->text() << std::endl;
		return ss.str();
	}

	const Map& map() const override { return _members; }

protected:
	std::shared_ptr<ScriptObject> _object;
	const std::string& _fullPath;
	Map _members;
};

class PointerValue : public Scriptable::Detail::ValueMember
{
public:
	PointerValue(Scriptable::Value *parentValue, std::wstring &&value)
	{
		_parentValue = parentValue;
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
		_value = convert.to_bytes(value);
		if (isOnMainThread)
			finalizeOnMainThread();
	}

#ifdef WIPSCRIPT
	void finalizeOnMainThread() override
	{
		auto guard = Context<std::string, MX::KeyContext>::Lock(const_cast<std::string &>(_parentValue->fullPath()));
		_pointer = Script::get().object(_value);
		_value.clear();
	}
#endif

	bool isConstant() const override { return _pointer->member()->isConstant(); }

	std::shared_ptr<Scriptable::Value> self() override { return _pointer; }
	bool isTrue() override { return _pointer->member()->isTrue(); }
	float valueFloat() override { return _pointer->member()->valueFloat(); }
	std::string valueString() override { return _pointer->member()->valueString(); }
	std::wstring valueWstring() override { return _pointer->member()->valueWstring(); }

	std::shared_ptr<Scriptable::Value> at(size_t index) override { return _pointer->member()->at(index); }
	std::shared_ptr<ScriptObject> object() override { return _pointer->member()->object(); }
	std::shared_ptr<ScriptObject> createObject() override { return _pointer->member()->createObject(); }

	size_t size() const override { return _pointer->member()->size(); }
	const Vector& array() const  override { return _pointer->member()->array(); }
protected:
	Scriptable::Value* _parentValue;
	std::string _value;
	std::shared_ptr<Scriptable::Value> _pointer;
};

bool Scriptable::Detail::ValueMember::isOnMainThread = true;




Scriptable::Value::Value(const std::string &fullPath) : _fullPath(fullPath)
{
	_member.reset(new Scriptable::Detail::ValueMember());
}



std::string Scriptable::Value::path()
{
	std::string path;
	size_t size = _fullPath.find_last_of('.');
	if (size != std::string::npos && size > 0)
		path = _fullPath.substr(0, size);
	return path;
}

void Scriptable::Value::UpdateObject(const Map& map)
{
	_member.reset(new ObjectValue(_fullPath, map));
}

void Scriptable::Value::Update(bool value)
{
	_member.reset(new BoolValue(value));
}
void Scriptable::Value::Update(float number, bool percent)
{
	if (percent)
		_member.reset(new PercentValue(number));
	else
		_member.reset(new FloatValue(number));
}

void Scriptable::Value::Update(const std::function<float()> &number_functor)
{
	_member.reset(new FunctorValue(number_functor));
}

void Scriptable::Value::Update(const std::wstring& string)
{
    if (!string.empty() && string[0] == L'&')
    {
		_member.reset(new PointerValue(this, string.substr(1)));
		return;
    }

#ifdef WIPSCRIPT
	if (!string.empty() && string[0] == L'=')
	{
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
		auto str = convert.to_bytes(string.substr(1));
		_member.reset(new RPNValue(this, std::move(str)));
		return;
	}
#endif

	_member.reset(new StringValue(this, string));
}


void Scriptable::Value::Update(const Vector& members)
{
	_member.reset(new ArrayValue(members));	
}


void Scriptable::Value::Update(const std::shared_ptr<ScriptObject>& object)
{
	_member.reset(new ObjectValue(_fullPath, object));	
}

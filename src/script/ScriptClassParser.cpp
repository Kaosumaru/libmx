#include "ScriptClassParser.h"
#include "Script.h"

#if WIP
#include "Class/MXScriptAnimationClass.h"
#include "Class/MXScriptSoundClass.h"
#include "Class/MXScriptImageClass.h"
#endif

using namespace MX;

ScriptClassParser::ScriptClassParser()
{
	//TODO find better place for this
#if WIP
	_AddCreator(L"Animation.AnimationFromFiles", new DefaultClassCreator<ScriptAnimationFromFilesClass>());
	_AddCreator(L"Animation.AnimationFromFile", new DefaultClassCreator<ScriptAnimationFromFileClass>());
	_AddCreator(L"Animation.RandomAnimation", new DefaultClassCreator<ScriptRandomAnimation>());

	_AddCreator(L"Sound.Sample", new DefaultClassCreator<ScriptSoundClass>());
	_AddCreator(L"Sound.RandomSample", new DefaultClassCreator<ScriptRandomSound>());

	_AddCreator(L"Image.Image", new DefaultClassCreator<ScriptImageClass>());
	_AddCreator(L"Image.Slice9Image", new DefaultClassCreator<ScriptSlice9ImageClass>());
	_AddCreator(L"Image.RandomImage", new DefaultClassCreator<ScriptRandomImage>());

	_AddCreator(L"Image.ImageGrid", new DefaultClassCreator<ScriptImageGridClass>());
	_AddCreator(L"Image.ImageFromGrid", new DefaultClassCreator<ScriptImageFromGridClass>());
#endif
}

ScriptClassParser::~ScriptClassParser()
{
}



const Scriptable::Value::pointer& ScriptClassParser::Parse(const std::string &instancePath)
{
	return ScriptClassParser::get()._Parse(instancePath);
}

const Scriptable::Value::pointer& ScriptClassParser::_Parse(const std::string &instancePath)
{
	static Scriptable::Value::pointer dummy;

	std::wstring className = Script::valueOf(instancePath + ".Class");

	auto it = _creators.find(className);
	if (it == _creators.end())
	{
		assert(false);
		return dummy;
	}
		
	auto object = it->second->createSharedPtr(className, instancePath);
	if (object)
	{
		auto &ret = Script::object(instancePath);
		ret->Update(object);
		return ret;
	}
	assert(false);
	return dummy;
}



std::shared_ptr<ScriptObject> ScriptClassParser::_CreateObject(const std::wstring &className, const std::string &instancePath)
{
	auto it = _creators.find(className);
	if (it == _creators.end())
	{
		assert(false);
		return nullptr;
	}
	return it->second->createSharedPtr(className, instancePath);
}

std::shared_ptr<ScriptObject> ScriptClassParser::_CreateObjectFromTemplate(const std::string &instancePath)
{
	auto& obj = Script::propertyOrNull(instancePath, "Object");
	std::wstring className;
	if (obj)
		className = obj->text();
	return _CreateObject(className, instancePath);
}

ScriptObject* ScriptClassParser::_CreateRawObjectFromTemplate(const std::string &instancePath)
{
	auto& obj = Script::propertyOrNull(instancePath, "Object");
	std::wstring className;
	if (obj)
		className = obj->text();

	auto it = _creators.find(className);
	if (it == _creators.end())
	{
		assert(false);
		return nullptr;
	}
	return it->second->createRaw(className, instancePath);
}


void ScriptClassParser::AddCreator(const std::wstring &className, CreatorBase* creator)
{
	ScriptClassParser::get()._AddCreator(className, creator);
}

void ScriptClassParser::AddCreator(const std::wstring &className, const ScriptClassCreator& creator)
{
	ScriptClassParser::get()._AddCreator(className, creator);
}

void ScriptClassParser::_AddCreator(const std::wstring &className, const ScriptClassCreator& creator)
{
	_AddCreator(className, new FunctionalClassCreator(creator));
}

void ScriptClassParser::_AddCreator(const std::wstring &className, CreatorBase* creator)
{
	_creators[className].reset(creator);
	_AddType(creator->type(), className);
}

std::wstring ScriptClassParser::GetType(std::type_index type)
{
	auto it = ScriptClassParser::get()._typeNameToCreatorString.find(type);
	if (it == ScriptClassParser::get()._typeNameToCreatorString.end())
		return L"";
	return it->second;
}

void ScriptClassParser::_AddType(std::type_index type, const std::wstring &className)
{
	if (type == std::type_index(typeid(void)))
		return;

	auto result = _typeNameToCreatorString.insert(std::make_pair(type, className));

	if (!result.second)
		assert(result.first->second == className);
}
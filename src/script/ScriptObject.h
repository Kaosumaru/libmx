#pragma once
#include "Script.h"
#include "serialization/Node.h"
#include <sstream>
#include <string>

namespace MX
{

struct PropertyLoader_Standard
{
};
struct PropertyLoader_Custom
{
};

template <typename T, class Enable = void>
struct PropertyLoader
{
    using type = PropertyLoader_Standard;
    static bool load(T& out, const Scriptable::Value::pointer& obj)
    {
        out = *obj;
        return true;
    }
};

template <>
struct PropertyLoader<std::string>
{
    using type = PropertyLoader_Standard;
    static bool load(std::string& out, const Scriptable::Value::pointer& obj)
    {
        out = obj->atext();
        return true;
    }
};

class ScriptString
{
public:
    explicit ScriptString(const char* label);
    explicit ScriptString(const std::string& label);
    explicit ScriptString(std::string&& label);
    explicit ScriptString(ScriptString&& Script);
    operator const std::wstring() const;
    operator const std::string() const;

    std::wstring text() const;
    std::string atext() const;

protected:
    std::string _data;
    const char* _label;
};

class ScriptObject
{
public:
    virtual ~ScriptObject();

    const Scriptable::Value::pointer& property_object(const std::string& name);

    virtual void operator&(Serialization::Node&& n)
    {
    }

#ifdef WIPSERIALIZE

    void Serialize()
    {
#ifdef _DEBUG
        bool deserializing = MX::Scriptable::Variable::currentlyDeserializing();
        assert(!deserializing);
#endif

        MX::Scriptable::SerializationMode mode(true);
        auto guard = mode.Use();
        auto lock = MX::Scriptable::Variable::scopedTransaction();
        *this& Scriptable::Variable(_serializePath.empty() ? object() : _serializePath);
    }

    void Deserialize()
    {
#ifdef _DEBUG
        bool serializing = MX::Scriptable::Variable::currentlySerializing();
        assert(!serializing);
#endif

        MX::Scriptable::SerializationMode mode(false);
        auto guard = mode.Use();
        auto lock = MX::Scriptable::Variable::scopedTransaction();
        *this& Scriptable::Variable(_serializePath.empty() ? object() : _serializePath);
    }
#endif

    virtual const std::string& object();
    virtual std::wstring name();
    virtual std::wstring description();

    std::string pathToChild(const std::string& name) { return object() + "." + name; }
};

template <typename ParentType>
class AddPropertyLoaders : public ParentType
{
public:
    AddPropertyLoaders()
    {
    }

    AddPropertyLoaders(const AddPropertyLoaders& other)
        : ParentType(other)
    {
    }

    template <typename T>
    const bool load_property_children(T& t, const std::string& name)
    {
        using loader_type = typename PropertyLoader<T>::type;
        if (load_property(loader_type(), t, name))
            return true;
        return load_property(loader_type(), t, "Children");
    }

    template <typename T>
    const bool load_property_child(T& t, const std::string& name)
    {
        using loader_type = typename PropertyLoader<T>::type;
        if (load_property(loader_type(), t, name))
            return true;
        return load_property(loader_type(), t, "Children.0");
    }

    template <typename T>
    const bool load_property(T& t, const std::string& name)
    {
        using loader_type = typename PropertyLoader<T>::type;
        return load_property(loader_type(), t, name);
    }

    template <typename T>
    const bool load_property(T& t, const std::string& name, const T& default_value)
    {
        if (load_property(t, name))
            return true;

        t = default_value;
        return false;
    }

protected:
    template <typename T>
    bool load_property(const PropertyLoader_Standard& st, T& t, const std::string& name)
    {
        auto prop = ParentType::property_object(name);
        if (prop)
        {
            return PropertyLoader<T>::load(t, prop);
        }
        return false;
    }

    template <typename T>
    bool load_property(const PropertyLoader_Custom& st, T& t, const std::string& name)
    {
        auto p = ParentType::property_object(name);
        if (!p)
            return false;
        return PropertyLoader<T>::load(t, *p);
    }
};

class LoadableScriptObject : public AddPropertyLoaders<ScriptObject>
{
};

using LScriptObject = LoadableScriptObject;

#if 0 //TODO probably has to be deprecated
class ScriptObjectString : public ScriptObject
{
public:
	ScriptObjectString(const ScriptObjectString& other) {}
	ScriptObjectString() {}
	ScriptObjectString(const std::string& objectName) : _objectName(objectName) {}

	void SetObjectName(const std::string& objectName)
	{
		_objectName = objectName;
	}

	std::string object()
	{
		return _objectName;
	}

protected:
	std::string _objectName;
};
#else
class ScriptObjectString : public LoadableScriptObject
{
public:
    ScriptObjectString() { }
    ScriptObjectString(const ScriptObjectString& other)
        : _objectName(other._objectName)
    {
    }
    ScriptObjectString(const std::string& objectName) { _objectName = std::make_shared<std::string>(objectName); }

    void SetObjectName(const std::string& objectName)
    {
        if (!_objectName)
            _objectName = std::make_shared<std::string>(objectName);
        else
            *_objectName = objectName;
    }

    const std::string& object() override
    {
        if (_objectName)
            return *_objectName;
        return LoadableScriptObject::object();
    }

protected:
    std::shared_ptr<std::string> _objectName;
};
#endif

#ifdef WIPSERIALIZE
class ScriptSettings : public ScriptObjectString
{
public:
    ScriptSettings(const std::string& objectName, const std::string& prefix = "App.Settings.");
    ~ScriptSettings();

    Scriptable::Variable variable(const std::string& name) override;

protected:
    bool _started = false;
};
#endif

}

std::wostream& operator<<(std::wostream& out, MX::ScriptString const& t);

#pragma once
#include "ScriptClass.h"
#include "script/PropertyLoaders.h"
#include "script/serialization/Node.h"
#include "utils/Utils.h"
#include <string>
#include <typeindex>

namespace MX
{

template <typename T>
struct DefaultClassCreatorContructor_Policy
{
    static std::shared_ptr<T> Create(const std::wstring& className, const std::string& instancePath)
    {
        return std::make_shared<T>(instancePath);
    };

    static T* CreateRaw(const std::wstring& className, const std::string& instancePath)
    {
        return new T(instancePath);
    };
};

template <typename T>
struct OutsideScriptClassCreatorContructor_Policy
{
    static std::shared_ptr<T> Create(const std::wstring& className, const std::string& instancePath)
    {
        ScriptObjectString script(instancePath);
        return std::make_shared<T>(script);
    };

    static T* CreateRaw(const std::wstring& className, const std::string& instancePath)
    {
        ScriptObjectString script(instancePath);
        return new T(script);
    };
};

template <typename T>
struct NoneClassCreatorContructor_Policy
{
    static std::shared_ptr<T> Create(const std::wstring& className, const std::string& instancePath)
    {
        return std::make_shared<T>();
    };

    static T* CreateRaw(const std::wstring& className, const std::string& instancePath)
    {
        return new T();
    };
};

class CreatorBase
{
public:
    using ScriptClassCreator = std::function<std::shared_ptr<ScriptObject>(const std::wstring& className, const std::string& instancePath)>;
    using ScriptObjectPtr = std::shared_ptr<ScriptObject>;

    virtual ~CreatorBase() {};
    virtual std::type_index type() const { return std::type_index(typeid(void)); }

    virtual ScriptObjectPtr createSharedPtr(const std::wstring& className, const std::string& instancePath)
    {
        assert(false);
        return nullptr;
    }
    virtual ScriptObject* createRaw(const std::wstring& className, const std::string& instancePath)
    {
        assert(false);
        return nullptr;
    }
};

template <typename T>
class CreatorTemplatedBase : public CreatorBase
{
public:
    std::type_index type() const override { return std::type_index(typeid(T)); }
};

template <typename T, template <typename> class Policy>
class CreatorTemplatedBase_Policy : public CreatorTemplatedBase<T>
{
public:
    CreatorBase::ScriptObjectPtr createSharedPtr(const std::wstring& className, const std::string& instancePath) override
    {
        return Policy<T>::Create(className, instancePath);
    }

    ScriptObject* createRaw(const std::wstring& className, const std::string& instancePath) override
    {
        return Policy<T>::CreateRaw(className, instancePath);
    }
};

class FunctionalClassCreator : public CreatorBase
{
public:
    FunctionalClassCreator(const ScriptClassCreator& creator)
        : _creator(creator)
    {
    }

    ScriptObjectPtr createSharedPtr(const std::wstring& className, const std::string& instancePath) override
    {
        return _creator(className, instancePath);
    }

protected:
    ScriptClassCreator _creator;
};

template <typename T>
class DefaultClassCreator : public CreatorTemplatedBase<T>
{
public:
    static std::shared_ptr<ScriptObject> Create(const std::wstring& className, const std::string& instancePath)
    {
        auto obj = std::make_shared<T>();
        if (obj->Parse(className, instancePath))
            return obj;
        return nullptr;
    }

    CreatorBase::ScriptObjectPtr createSharedPtr(const std::wstring& className, const std::string& instancePath) override
    {
        return Create(className, instancePath);
    }

    ScriptObject* createRaw(const std::wstring& className, const std::string& instancePath) override
    {
        auto obj = new T();
        if (obj->Parse(className, instancePath))
            return obj;
        return nullptr;
    }
};

template <typename T>
class DefaultClassCreatorContructor : public CreatorTemplatedBase_Policy<T, DefaultClassCreatorContructor_Policy>
{
};

template <typename T>
class OutsideScriptClassCreatorContructor : public CreatorTemplatedBase_Policy<T, OutsideScriptClassCreatorContructor_Policy>
{
};

template <typename T>
class NoneClassCreatorContructor : public CreatorTemplatedBase_Policy<T, NoneClassCreatorContructor_Policy>
{
};

template <typename T, template <typename> class Policy = DefaultClassCreatorContructor_Policy>
class DefaultCachedClassCreatorContructor : public CreatorTemplatedBase<T>
{
public:
    using pointer = std::shared_ptr<T>;
    using CloneFunction = std::function<pointer(const pointer&)>;
    using Cache = std::map<std::string, pointer>;

    DefaultCachedClassCreatorContructor(const CloneFunction& cloneFunc)
        : _cloneFunction(cloneFunc)
    {
        Script::onParsed().connect([&]() {
            _cache.clear();
        });
    }

    CreatorBase::ScriptObjectPtr createSharedPtr(const std::wstring& className, const std::string& instancePath) override
    {
        auto& obj = _cache[instancePath];
        if (!obj)
            obj = Policy<T>::Create(className, instancePath);

        return _cloneFunction(obj);
    }

protected:
    Cache _cache;
    CloneFunction _cloneFunction;
};

namespace Scriptable
{
    class Value;
}

class ScriptClassParser : public Singleton<ScriptClassParser>
{
public:
    using ScriptClassCreator = std::function<std::shared_ptr<ScriptObject>(const std::wstring& className, const std::string& instancePath)>;
    using CreatorsMap = std::map<std::wstring, std::unique_ptr<CreatorBase>>;

    ScriptClassParser();
    virtual ~ScriptClassParser();

    static const std::shared_ptr<Scriptable::Value>& Parse(const std::string& instancePath);
    static std::shared_ptr<ScriptObject> CreateObjectFromTemplate(const std::string& instancePath)
    {
        return ScriptClassParser::get()._CreateObjectFromTemplate(instancePath);
    }

    template<typename T>
    static std::shared_ptr<T> CreateFromTemplate(const std::string& instancePath)
    {
        auto obj = ScriptClassParser::get()._CreateObjectFromTemplate(instancePath);
        return std::dynamic_pointer_cast<T>(obj);
    }

    static ScriptObject* CreateRawObjectFromTemplate(const std::string& instancePath)
    {
        return ScriptClassParser::get()._CreateRawObjectFromTemplate(instancePath);
    }

    static void AddCreator(const std::wstring& className, const ScriptClassCreator& creator);
    static void AddCreator(const std::wstring& className, CreatorBase* creator);

    template <typename T>
    static std::shared_ptr<T> CreateType(const std::wstring& className, const std::string& instancePath)
    {
        return std::dynamic_pointer_cast<T>(ScriptClassParser::get()._CreateObject(className, instancePath));
    }

    static std::wstring GetType(std::type_index);

protected:
    const std::shared_ptr<Scriptable::Value>& _Parse(const std::string& className);

    std::shared_ptr<ScriptObject> _CreateObject(const std::wstring& className, const std::string& instancePath);
    std::shared_ptr<ScriptObject> _CreateObjectFromTemplate(const std::string& instancePath);
    ScriptObject* _CreateRawObjectFromTemplate(const std::string& instancePath);

    void _AddCreator(const std::wstring& className, const ScriptClassCreator& creator);
    void _AddCreator(const std::wstring& className, CreatorBase* creator);
    void _AddType(std::type_index type, const std::wstring& className);

    CreatorsMap _creators;
    std::unordered_map<std::type_index, std::wstring> _typeNameToCreatorString;
};

}

template <typename T>
void operator&(std::shared_ptr<T>& t, MX::Serialization::Node&& var)
{
    if (var.saving())
    {
        if (!t)
        {
            std::wstring className = L"";
            className& var("Object");
            return;
        }

        auto& data = *(t.get());
        std::wstring className = MX::ScriptClassParser::GetType(typeid(data));
        assert(!className.empty());
        className& var("Object");
        const_cast<std::string&>(data.object()) & var("InstancePath");
        //data.SetSerializePath(var.fullPath());
        data.operator&(std::move(var));
    }
    else
    {
        std::wstring className;
        std::string instancePath;
        className& var("Object");
        instancePath& var("InstancePath");
        if (className.empty())
        {
            t = nullptr;
            return;
        }

        t = MX::ScriptClassParser::CreateType<T>(className, instancePath);
        auto& data = *(t.get());
        data.operator&(std::move(var));
    }
}

#define MXTOKENPASTE(x, y) x##y
#define MXTOKENPASTE2(x, y) MXTOKENPASTE(x, y)

#define MXREGISTER_CLASS(strname, classname)                                                                             \
    namespace                                                                                                            \
    {                                                                                                                    \
        struct MXTOKENPASTE2(Autoregister, __LINE__)                                                                     \
        {                                                                                                                \
            MXTOKENPASTE2(Autoregister, __LINE__)                                                                        \
            () { MX::ScriptClassParser::AddCreator(strname, new MX::OutsideScriptClassCreatorContructor<classname>()); } \
        } MXTOKENPASTE2(autoregister_inst, __LINE__);                                                                    \
    }

#define MXREGISTER_CLASS_DEFAULT(strname, classname)                                                               \
    namespace                                                                                                      \
    {                                                                                                              \
        struct MXTOKENPASTE2(Autoregister, __LINE__)                                                               \
        {                                                                                                          \
            MXTOKENPASTE2(Autoregister, __LINE__)                                                                  \
            () { MX::ScriptClassParser::AddCreator(strname, new MX::DefaultClassCreatorContructor<classname>()); } \
        } MXTOKENPASTE2(autoregister_inst, __LINE__);                                                              \
    }


#define MXREGISTER_SCRIPT(CODE_VAR)                                \
    namespace                                                  \
    {                                                          \
        struct MXTOKENPASTE2(Autoregister, __LINE__)           \
        {                                                      \
            MXTOKENPASTE2(Autoregister, __LINE__)              \
            ()                                                 \
            {                                                  \
                auto fnct = [&]() CODE_VAR ;                    \
                Script::onParsed().static_connect(fnct); \
                Script::BeforeFirstParse().static_connect(fnct); \
            }                                                  \
        } MXTOKENPASTE2(autoregister_inst, __LINE__);          \
    }
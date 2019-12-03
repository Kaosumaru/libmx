#ifndef MXScriptableVariable
#define MXScriptableVariable
#include "Graphic/MXColor.h"
#include "Utils/MXUtils.h"
#include "Utils/MXVector2.h"
#include "utils/SignalizingVariable.h"
#include <codecvt>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace MX
{

class ScriptParser;
class ScriptObject;

namespace Scriptable
{
    class VariableSerializer : public ScopeSingleton<VariableSerializer>
    {
    public:
        ~VariableSerializer() { }

        enum VariableType
        {
            Variable,
            Array,
            Map
        };

        VariableType typeOf(const std::string& path)
        {
            std::string str;
            if (!Load(path + ".Type", str))
                return Variable;
            return stringToType(str);
        }

        virtual void Save(const std::string& path, bool value) = 0;
        virtual void Save(const std::string& path, float value) = 0;
        virtual void Save(const std::string& path, int value) = 0;
        virtual void Save(const std::string& path, const std::string& value) = 0;
        virtual void Save(const std::string& path, const std::wstring& value) = 0;

        virtual bool Load(const std::string& path, bool& value) = 0;
        virtual bool Load(const std::string& path, float& value) = 0;
        virtual bool Load(const std::string& path, int& value) = 0;
        virtual bool Load(const std::string& path, std::string& value) = 0;
        virtual bool Load(const std::string& path, std::wstring& value) = 0;

        struct AutoTransaction
        {
            AutoTransaction(VariableSerializer& s)
                : _s(s)
            {
                _s.BeginTransaction();
            }
            ~AutoTransaction() { _s.CommitTransaction(); }

        protected:
            VariableSerializer& _s;
        };

        friend struct AutoTransaction;
        AutoTransaction scopedTransaction() { return AutoTransaction(*this); }

        virtual void BeginTransaction() { assert(false); }
        virtual void CommitTransaction() { assert(false); }

        void Remove(const std::string& path)
        {
            auto type = typeOf(path);
            switch (type)
            {
            case Variable:
                return InternalRemoveVariable(path);
            case Array:
                return InternalRemoveArray(path);
            case Map:
                return InternalRemoveMap(path);
            }
        }

        virtual void RemovePath(const std::string& path) = 0;

        void saveType(const std::string& path, VariableType type)
        {
            Save(path + ".Type", typeToString(type));
        }

    protected:
        void RemoveIfComplexType(const std::string& path)
        {
            auto type = typeOf(path);
            switch (type)
            {
            case Variable:
                return;
            case Array:
                return InternalRemoveArray(path);
            case Map:
                return InternalRemoveMap(path);
            }
        }

        virtual void InternalRemoveVariable(const std::string& path) = 0;
        virtual void InternalRemoveArray(const std::string& path)
        {
            int size = 0;
            Load(path + ".Count", size);

            InternalRemoveVariable(path + ".Type");
            InternalRemoveVariable(path + ".Count");

            for (int i = 0; i < size; i++)
                InternalRemoveVariable(path + "." + std::to_string(i));
        }
        virtual void InternalRemoveMap(const std::string& path)
        {
            int size = 0;
            Load(path + ".PairCount", size);

            InternalRemoveVariable(path + ".Type");
            InternalRemoveVariable(path + ".PairCount");

            for (int i = 0; i < size; i++)
            {
                InternalRemoveVariable(path + ".0." + std::to_string(i));
                InternalRemoveVariable(path + ".1." + std::to_string(i));
            }
        }

        VariableType stringToType(const std::string& str)
        {
            static std::map<std::string, VariableType> m = { { "Variable", Variable }, { "Array", Array }, { "Map", Map } };
            auto it = m.find(str);
            if (it == m.end())
                return Variable;
            return it->second;
        }

        std::string typeToString(const VariableType& type)
        {
            static std::map<VariableType, std::string> m = { { Variable, "Variable" }, { Array, "Array" }, { Map, "Map" } };
            auto it = m.find(type);
            if (it == m.end())
                return "Variable";
            return it->second;
        }
    };

    struct SerializationMode : ScopeSingleton<SerializationMode>
    {
        SerializationMode() { }
        SerializationMode(bool serializing_)
            : serializing(serializing_)
        {
        }

        bool serializing = false;
    };

    class Variable : public shared_ptr_init<Variable>
    {
    protected:
        template <typename T>
        T load() const
        {
            T ret = T();
            VariableSerializer::current().Load(_fullPath, ret);
            return ret;
        }

        float load() const
        {
            float ret = 0.0f;
            VariableSerializer::current().Load(_fullPath, ret);
            return ret;
        }

    public:
        void Remove()
        {
            VariableSerializer::current().Remove(_fullPath);
        }

        void saveType(VariableSerializer::VariableType type)
        {
            VariableSerializer::current().saveType(_fullPath, type);
        }

        //TODO hacky
        template <typename T>
        bool load(const T& t)
        {
            assert(false);
            return false;
        }

        template <typename T>
        bool load(T& t)
        {
            return VariableSerializer::current().Load(_fullPath, t);
        }

        template <typename T>
        void save(const T& t)
        {
            VariableSerializer::current().Save(_fullPath, t);
        }

        const bool isTrue() const
        {
            return load<float>() != 0.0f;
        }

        template <typename T>
        operator T() const
        {
            return load<T>();
        }

        template <typename T>
        auto& operator=(const T& t)
        {
            save(t);
            return *this;
        }

        static bool currentlySerializing()
        {
            return ScopeSingleton<MX::Scriptable::SerializationMode>::isCurrent() && ScopeSingleton<MX::Scriptable::SerializationMode>::current().serializing;
        }

        static bool currentlyDeserializing()
        {
            return ScopeSingleton<MX::Scriptable::SerializationMode>::isCurrent() && !ScopeSingleton<MX::Scriptable::SerializationMode>::current().serializing;
        }

        Variable(const std::string& label);
        static VariableSerializer::AutoTransaction scopedTransaction() { return VariableSerializer::current().scopedTransaction(); }

        const std::string& fullPath() const
        {
            return _fullPath;
        }

        Variable operator()(const std::string& child)
        {
            return Variable(_fullPath + "." + child);
        }

    protected:
        bool _needsRecalc;
        std::string _path;
        std::string _fullPath;
    };

}

}

template <typename T>
void operator&(T& t, MX::Scriptable::Variable& var)
{
    if (MX::Scriptable::Variable::currentlySerializing())
        var.save(t);
    else
    {
        var.load(t);
    }
}

template <typename T>
void operator&(T& t, MX::Scriptable::Variable&& var)
{
    if (MX::Scriptable::Variable::currentlySerializing())
        var.save(t);
    else
    {
        var.load(t);
    }
}

template <typename T>
void operator&(SignalizingVariable<T>& t, MX::Scriptable::Variable&& var)
{
    if (!MX::Scriptable::Variable::currentlySerializing())
    {
        T old = t.directValueAccess();
        t.directValueAccess() & std::move(var);
        t.onValueChanged(t.directValueAccess(), old);
    }
    else
    {
        t.directValueAccess() & std::move(var);
    }
}

void operator&(MX::Vector2& t, MX::Scriptable::Variable&& var);

namespace detail
{
template <typename T1, typename T2, typename T>
void transferMap(T& m, MX::Scriptable::Variable&& var)
{
    if (MX::Scriptable::Variable::currentlySerializing())
    {
        var.Remove();
        var.saveType(MX::Scriptable::VariableSerializer::Map);
        int size = (int)m.size();
        size& var("PairCount");
        int i = 0;
        for (auto& pair : m)
        {
            pair.first& var("0." + std::to_string(i));
            pair.second& var("1." + std::to_string(i));
            i++;
        }
    }
    else
    {
        int size = 0;
        size& var("PairCount");

        T1 key;
        T2 value;
        for (int i = 0; i < size; i++)
        {
            key& var("0." + std::to_string(i));
            value& var("1." + std::to_string(i));
            m.emplace(std::move(key), std::move(value));
        }
    }
}

template <typename ItemType, typename T>
void transferArray(T& arr, MX::Scriptable::Variable&& var)
{
    if (MX::Scriptable::Variable::currentlySerializing())
    {
        var.Remove();
        var.saveType(MX::Scriptable::VariableSerializer::Array);

        int size = (int)arr.size();
        size& var("Count");

        int i = 0;
        for (auto& item : arr)
        {
            item& var(std::to_string(i++));
        }
    }
    else
    {
        int size = 0;
        size& var("Count");

        arr.clear();

        ItemType obj;

        for (int i = 0; i < size; i++)
        {
            obj& var(std::to_string(i));
            arr.push_back(obj);
        }
    }
}

};

template <typename T1, typename T2>
void operator&(std::multimap<T1, T2>& m, MX::Scriptable::Variable&& var)
{
    ::detail::transferMap<T1, T2>(m, std::move(var));
}

template <typename T1, typename T2>
void operator&(std::map<T1, T2>& m, MX::Scriptable::Variable&& var)
{
    ::detail::transferMap<T1, T2>(m, std::move(var));
}

template <typename T1, typename T2>
void operator&(std::unordered_map<T1, T2>& m, MX::Scriptable::Variable&& var)
{
    ::detail::transferMap<T1, T2>(m, std::move(var));
}

template <typename T>
void operator&(std::list<T>& arr, MX::Scriptable::Variable&& var)
{
    ::detail::transferArray<T>(arr, std::move(var));
}

template <typename T>
void operator&(std::vector<T>& arr, MX::Scriptable::Variable&& var)
{
    ::detail::transferArray<T>(arr, std::move(var));
}

#endif

#pragma once
#include <string>
#include <unordered_map>

namespace MX
{
namespace Widgets
{

    class PropertyMap
    {
    protected:
        struct Variant
        {
            float _f = 0.0f;
            std::string _s;
            bool _string = false;
        };

    public:
        void SetValue(const std::string& name, const float& v)
        {
            auto& var = _values[name];
            var._f = v;
            var._s.clear();
            var._string = false;
        }
        void SetValue(const std::string& name, const std::string& v)
        {
            auto& var = _values[name];
            var._f = 0.0f;
            var._s = v;
            var._string = true;
        }

        template <typename T>
        T valueOf(const std::string& name, const T& def = {}) const
        {
            auto it = _values.find(name);
            if (it == _values.end())
                return def;
            if (!it->second._string)
                return it->second._f;
            return def;
        }

    protected:
        std::unordered_map<std::string, Variant> _values;
    };

    template <>
    inline std::string PropertyMap::valueOf(const std::string& name, const std::string& def) const
    {
        auto it = _values.find(name);
        if (it == _values.end())
            return def;
        if (it->second._string)
            return it->second._s;
        return def;
    }

}

}

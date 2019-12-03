#pragma once

#include <memory>
#include <typeindex>

namespace MX
{
template <typename T = void>
class ClassID
{
public:
    using type = std::type_index;
    static inline type& id()
    {
        static std::type_index t = std::type_index(typeid(T));
        return t;
    }
};
}
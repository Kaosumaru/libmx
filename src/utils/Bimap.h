#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <vector>

namespace MX
{
template <typename T1, typename T2, typename Data>
class Bimap
{
public:
    struct Element
    {
        std::pair<T1, T2> key;
        Data value;

        bool operator<(const Element& e) const
        {
            return key < e.key;
        }
    };

    using Container = std::vector<Element>;

    auto& insert(T1 t1, T2 t2)
    {
        Element e;
        e.key.first = t1;
        e.key.second = t2;

        auto it = std::lower_bound(_elements.begin(), _elements.end(), e);
        if (it != _elements.end() && it->key == e.key)
            return *it;

        return *(_elements.insert(it, e));
    }

    auto begin()
    {
        return _elements.begin();
    }

    auto end()
    {
        return _elements.end();
    }

    auto find(T1 t1, T2 t2)
    {
        Element e;
        e.key.first = t1;
        e.key.second = t2;
        auto it = std::lower_bound(_elements.begin(), _elements.end(), e);

        if (it != _elements.end() && it->key == e.key)
            return it;
        return _elements.end();
    }

    auto erase(typename Container::iterator it)
    {
        return _elements.erase(it);
    }

    template <typename C>
    void enumerate(T1 value, C callback)
    {
        for (auto& v : _elements)
        {
            if (v.key.first == value)
            {
                callback(v.key.second, v);
                continue;
            }
            if (v.key.second == value)
            {
                callback(v.key.first, v);
                continue;
            }
        }
    }

    template <typename C>
    void remove_if(C callback)
    {
        auto xit = std::remove_if(_elements.begin(), _elements.end(), callback);
        _elements.erase(xit, _elements.end());
    }

protected:
    Container _elements;
};
}

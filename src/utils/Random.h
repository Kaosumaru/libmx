#pragma once
#include <assert.h>
#include <map>
#include <memory>
#include <random>

class Random
{
public:
    static void Init();
    static void Seed(unsigned long s);

    static bool chance(float chance);

    static float randomRange(float range = 1.0f);

    template <typename T>
    static T randomRange(const std::pair<T, T>& pair)
    {
        return std::uniform_real_distribution<T>(pair.first, pair.second)(_rgn);
    }

    template <typename T>
    static T randomGaussRange(const std::pair<T, T>& pair)
    {
        return std::normal_distribution<T>(pair.first, pair.second)(_rgn);
    }

    template <typename T>
    static T randomInt(T min, T max)
    {
        return std::uniform_int_distribution<T>(min, max)(_rgn);
    }

    template <typename T>
    static T randomIntUpTo(T non_contained_max)
    {
        return std::uniform_int_distribution<T>((T)0, non_contained_max - 1)(_rgn);
    }

    template <typename T>
    static auto& randomFrom(const std::vector<T>& v)
    {
        return v[randomIntUpTo(v.size())];
    }

    template <typename T>
    static auto randomFromRange(T it_begin, T it_end)
    {
        auto i = Random::randomInt(std::ptrdiff_t { 0 }, std::distance(it_begin, it_end) - 1);
        std::advance(it_begin, i);
        return it_begin;
    }

    static unsigned randomRange(const std::pair<unsigned, unsigned>& pair);
    static int randomRange(const std::pair<int, int>& pair);

    typedef std::mt19937 MyRNG;
    static MyRNG _rgn;
};

template <typename Type>
class RandomItem
{
protected:
    typedef typename std::shared_ptr<Type> T;

public:
    RandomItem()
    {
        _last = 0.0f;
    }

    ~RandomItem() { }

    virtual void AddItem(const T& item, float weight = 1.0f)
    {
        assert(item);
        _last += weight;
        _items[_last] = item;
    }

    T& randomItem()
    {
        return _items.lower_bound(Random::randomRange(_last))->second;
    }

    bool empty() const
    {
        return _items.empty();
    }

    const auto& items() { return _items; }

protected:
    std::map<float, T> _items;
    float _last;
};

#if 0
template<typename Type>
class RandomNonLockedItem : public RandomItem<Type>
{
public:
	void AddItem(const T& item, float weight = 1.0f) override
	{
		if (MX::LockableManager::current().isLocked(item->object()))
			return;
		RandomItem<Type>::AddItem(item, weight);
	}
};
#endif
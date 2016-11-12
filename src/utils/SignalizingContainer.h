#pragma once
#include<memory>
#include "Signal.h"

template< typename T, template <class Created, class Alloc = std::allocator<Created> > class Container = std::list>
class SignalizingContainer
{
public:
	typedef Container<T> ContainerType;

	SignalizingContainer() {}
	SignalizingContainer(const ContainerType& l) : _list(l) {}

	Container::iterator begin()
	{
		return _list.begin();
	}
	Container::const_iterator begin() const
	{
		return _list.begin();
	}

	Container::iterator end()
	{
		return _list.end();
	}
	Container::const_iterator end() const
	{
		return _list.end();
	}


	const std::list<T>& list() { return _list; }

	void push_back(const T& t) { _list.push_back(t); _onChanged(); }
	void push_front(const T& t) { _list.push_front(t); _onChanged(); }
	void clear() { _list.clear(); _onChanged(); }

	void operator = (const ContainerType& l) { _list = l; _onChanged(); }

	default_signal<void(const ContainerType& container)> onChanged;
protected:
	void _onChanged()
	{
		onChanged(_list);
	}

	ContainerType _list;
};


template<typename T>
class SignalizingList : public SignalizingContainer<T, std::list>
{
public:
	SignalizingList() {}
	SignalizingList(const ContainerType& l) : SignalizingContainer(l) {}
};

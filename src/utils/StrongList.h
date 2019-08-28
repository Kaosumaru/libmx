#pragma once
#include<list>
#include<cassert>
#include<memory>
#include"ClassID.h"
#include<typeindex>

namespace MX
{


class List_Link
{
public:
	virtual ~List_Link()
	{
	
	}
	virtual void unlink() = 0;

	virtual ClassID<>::type type() { return ClassID<>::id(); }
};

template<typename T>
class StrongList_Link : public List_Link
{
public:
	StrongList_Link() {  }

    virtual void SetType(ClassID<>::type type)
	{
		_type = type;
	}

    virtual void Set(T& container, typename T::iterator it, const unsigned &iterating, bool &dirty, ClassID<>::type type = ClassID<>::id())
    {
        _container = &container;
        _it = it;
        _iterating = &iterating;
        _dirty = &dirty;
		if (type != ClassID<>::id())
			_type = type;
    }
    
	void unlink()
	{
		if (!*_iterating)
			_container->erase(_it);
		else
		{
			*_dirty = true;
			_it->reset();
		}
	}


	ClassID<>::type type() { return _type; }
protected:
	typename T::iterator _it;
	T* _container;
	const unsigned* _iterating;
	bool *          _dirty;
	ClassID<>::type _type = ClassID<>::id();
};



class LinkableSetter;

class Linkable
{
public:
	friend class LinkableSetter;

	virtual void Unlink()
	{
		if (_link)
		{
			auto link = _link.release();
			link->unlink();
			delete link;
		}
	}

	virtual bool linked()
	{
		return _link != nullptr;
	}

	List_Link* link()
	{
		return _link.get();
	}
protected:
	std::unique_ptr<List_Link> _link;
};


class LinkableSetter
{
protected:
	void SetLink(Linkable &linkable, List_Link *link)
	{
		linkable._link.reset(link);
	}
};


template<typename T>
class StrongList : public LinkableSetter
{
public:
	friend class StrongList_Link<T>;

	typedef typename std::shared_ptr<T> pointer;
	typedef typename std::list<T>::iterator iterator;
	typedef typename std::list<T>::iterator const_iterator;
	typedef T type;
	typedef typename std::list<pointer> ListType;


public:

	StrongList()
	{
		_iterating = 0;
		_dirty = false;
	}

	~StrongList()
	{
		orphan_list();
	}

	struct IterationGuard
	{
		IterationGuard(unsigned& iterating): _iterating(iterating) { ++_iterating; }
		~IterationGuard()  { --_iterating; }
		unsigned& _iterating;
	};

	friend struct IterationGuard;

	IterationGuard guard()
	{
		return IterationGuard(_iterating);
	}


	void push_back(const pointer& v, StrongList_Link<ListType>* link = 0)
	{
		_list.push_back(v);
        if (link == 0)
            link = new StrongList_Link<ListType>();
        link->Set(_list, --_list.end(), _iterating, _dirty);
		SetLink(*v, link);
	}

	void push_front(const pointer& v, StrongList_Link<ListType>* link = 0)
	{
		_list.push_front(v);
        if (link == 0)
            link = new StrongList_Link<ListType>();
        link->Set(_list, _list.begin(), _iterating, _dirty);
		SetLink(*v, link);
	}

	
	void temp_advance(typename ListType::iterator &it)
	{
		++it;
		while (true)
		{
			if (it == _list.end() || *it != nullptr)
				return;

			if (_iterating < 2)
				it = _list.erase(it);
			else
				it ++;
		}
	}

	void temp_advance(typename ListType::const_iterator &it)
	{
		++it;
		while (true)
		{
			if (it == _list.end() || *it != nullptr)
				return;
			if (_iterating < 2)
				it = _list.erase(it);
			else
				it ++;
		}
	}

	typename ListType::iterator begin()
	{
		auto it = _list.begin();
		while (it != _list.end() && *it == nullptr)
		{
			if (_iterating < 2)
				it = _list.erase(it);
			else
				it ++;
		}
		//TODO add assert here
		return it;
	}

	typename ListType::iterator end()
	{
		return _list.end();
	}

	typename ListType::const_iterator begin() const
	{
		if (_list.empty())
			return _list.end();
		auto it = _list.begin();
		while (it != _list.end() && *it == nullptr)
			it ++;
		//TODO add assert here
		return _list.begin();
	}

	typename ListType::const_iterator end() const
	{
		return _list.end();
	}

	unsigned size()
	{
		clean_list();
		return _list.size();
	}

	bool empty()
	{
		if (!_list.empty() && !_list.front())
			clean_list();
		return _list.empty();
	}

	typename StrongList::pointer& front()
	{
		clean_list();
		return _list.front();
	}

	typename StrongList::pointer& back()
	{
		clean_list();
		return _list.back();
	}

	void resize(unsigned size)
	{
		clean_list();
		_list.resize(size);
	}

	void clear()
	{
		assert(_iterating == 0);
		orphan_list();
		_list.clear();
	}

    template<typename X>
    void sort(const X &comp)
    {
        _list.sort(comp);
    }

protected:
	void clean_list()
	{
		if (!_dirty || _iterating > 0)
			return;
		auto it = _list.begin();
		while (it != _list.end())
		{
			if (*it == nullptr)
				it = _list.erase(it);
			else
				it++;
		}
		_dirty = false;
	}

	void orphan_list()
	{
		auto it = _list.begin();
		while (it != _list.end())
		{
			if (*it != nullptr)
				SetLink(**it, 0);
			it ++;
		}
	}

	unsigned _iterating;
	ListType _list;
	bool _dirty;
};



template<typename T>
class WeakList : public LinkableSetter
{
public:
	friend class StrongList_Link<T>;

	typedef typename std::shared_ptr<T> shared_pointer;
	typedef typename std::weak_ptr<T> pointer;
	typedef typename std::list<T>::iterator iterator;
	typedef typename std::list<T>::iterator const_iterator;
	typedef T type;
	typedef typename std::list<pointer> ListType;
	typedef StrongList_Link<ListType> LinkType;

public:

	WeakList()
	{
		_iterating = 0;
		_dirty = false;
	}

	~WeakList()
	{
		orphan_list();
	}

	struct IterationGuard
	{
		IterationGuard(unsigned& iterating): _iterating(iterating) { ++_iterating; }
		~IterationGuard()  { --_iterating; }
		unsigned& _iterating;
	};

	friend struct IterationGuard;

	IterationGuard guard()
	{
		return IterationGuard(_iterating);
	}


	void push_back(const shared_pointer& v, ClassID<>::type type = ClassID<>::id())
	{
		_list.push_back(v);
        auto link = new LinkType();
        link->Set(_list, --_list.end(), _iterating, _dirty, type);
		SetLink(*v, link);
	}

	void push_front(const shared_pointer& v, ClassID<>::type type = ClassID<>::id())
	{
		_list.push_front(v);
        auto link = new LinkType();
        link->Set(_list, _list.begin(), _iterating, _dirty, type);
		SetLink(*v, link);
	}
    
	void push_back(const shared_pointer& v, StrongList_Link<ListType>* link)
	{
		_list.push_back(v);
        if (link == 0)
            link = new LinkType();
        link->Set(_list, --_list.end(), _iterating, _dirty);
		SetLink(*v, link);
	}
    
	void push_front(const shared_pointer& v, StrongList_Link<ListType>* link)
	{
		_list.push_front(v);
        if (link == 0)
            link = new LinkType();
        link->Set(_list, _list.begin(), _iterating, _dirty);
		SetLink(*v, link);
	}



	void move_back(const shared_pointer& v)
	{
		_list.push_back(v);
        auto link = v->link();
		link->unlink();
        static_cast<LinkType*>(link)->Set(_list, --_list.end(), _iterating, _dirty);
	}
    
	void move_front(const shared_pointer& v)
	{
		_list.push_front(v);
        auto link = v->link();
		link->unlink();
        static_cast<LinkType*>(link)->Set(_list, _list.begin(), _iterating, _dirty);
	}

	
	void temp_advance(typename ListType::iterator &it)
	{
		++it;
		while (true)
		{
			if (it == _list.end() || !it->expired())
				return;
			if (_iterating < 2)
				it = _list.erase(it);
			else
				it ++;
		}
	}

	void temp_advance(typename ListType::const_iterator &it)
	{
		++it;
		while (true)
		{
			if (it == _list.end() || !it->expired())
				return;
			if (_iterating < 2)
				it = _list.erase(it);
			else
				it ++;
		}
	}

	typename ListType::iterator begin()
	{
		auto it = _list.begin();
		while (it != _list.end() && it->expired())
		{
			if (_iterating < 2)
				it = _list.erase(it);
			else
				it ++;
		}
		//TODO add assert here
		return it;
	}

	typename ListType::iterator end()
	{
		return _list.end();
	}

	typename ListType::const_iterator begin() const
	{
		auto it = _list.begin();
		while (it != _list.end() && it->expired())
		{
			if (_iterating < 2)
				it = _list.erase(it);
			else
				it ++;
		}
		//TODO add assert here
		return _list.begin();
	}

	typename ListType::const_iterator end() const
	{
		return _list.end();
	}

	auto size()
	{
		clean_list();
		return _list.size();
	}

	bool empty()
	{
		if (!_list.empty() && _list.begin()->expired())
			clean_list();
		return _list.empty();
	}

	typename WeakList::pointer& front()
	{
		clean_list();
		return _list.front();
	}

	typename WeakList::pointer& back()
	{
		clean_list();
		return _list.back();
	}

	void resize(unsigned size)
	{
		clean_list();
		_list.resize(size);
	}

	void clean_list()
	{
		if (!_dirty || 	_iterating > 0)
			return;
		auto it = _list.begin();
		while (it != _list.end())
		{
			if (it->expired())
				it = _list.erase(it);
			else
				it ++;
		}
		_dirty = false;
	}

protected:
	void orphan_list()
	{
		auto it = _list.begin();
		while (it != _list.end())
		{
			if (!it->expired())
				SetLink(*(it->lock()), 0);
			it ++;
		}    
	}

	unsigned _iterating;
	ListType _list;
	bool _dirty;
};

}


#pragma once

#include<memory>
#include<set>
#include "Singleton.h"

namespace MX
{
	template<typename T>
	std::weak_ptr<T> weaken(const std::shared_ptr<T>& ptr)
	{
		return std::weak_ptr<T>{ ptr };
	}

	template <typename F>
	struct ScopeExit {
		ScopeExit(F &&f) : f(std::forward<F>(f)) {}
		~ScopeExit() { f(); }
		F f;
	};

	template <typename F>
	ScopeExit<F> AtScopeExit(F&&f)
	{
		return ScopeExit<F>(std::forward<F>(f));
	}


	template <template<class, class, class...> class C, typename K, typename V, typename... Args>
	V get_or_default(const C<K, V, Args...>& m, K const& key, const V & defval)
	{
		typename C<K, V, Args...>::const_iterator it = m.find(key);
		if (it == m.end())
			return defval;
		return it->second;
	}

	template<typename T>
	void AssertPtrClear(std::shared_ptr<T>& ptr)
	{
		assert(ptr.use_count() <= 1);
		ptr = nullptr;
	}

	class disable_copy_constructors
	{
	private:
		disable_copy_constructors(disable_copy_constructors &){}
		disable_copy_constructors& operator =(disable_copy_constructors *a)
		{
			return *this;
		}
	public:
		disable_copy_constructors(){}
	};

	//transparent wrapper for c-style pointer, which need deinitializer function
	template <typename T, void (*destructor_function)(T*)>
	struct auto_deallocator : public virtual disable_copy_constructors
	{
	public:

		auto_deallocator()
		{
		}

		auto_deallocator(auto_deallocator &&other)
		{
			std::swap(_member, other._member);
		}


		~auto_deallocator()
		{
			if (_member)
				destructor_function(_member);
		}

		operator T *()
		{
			return _member;
		}

		operator const T *() const
		{
			return _member;
		}

		auto_deallocator& operator =(T *a)
		{
			if (_member == a)
				return *this;
			if (_member)
				destructor_function(_member);
			_member = a;
			return *this;
		}

		auto_deallocator& operator =(auto_deallocator &&other)
		{
			if (_member == other._member)
				return *this;
			if (_member)
				destructor_function(_member);
			_member = nullptr;
			std::swap(_member, other._member);
			return *this;
		}

	protected:
		T *_member = nullptr;
	};


	//base class
	template<typename T>
	struct shared_ptr_init : public std::enable_shared_from_this<T>
	{
		typedef std::shared_ptr<T> pointer;
		typedef std::weak_ptr<T> weak_pointer;

		template<typename X>
		typename std::shared_ptr<X> shared_to()
		{
			return std::static_pointer_cast<X>(std::enable_shared_from_this<T>::shared_from_this());
		}
	};



	template<typename T=void>
	class ClassID 
	{
		T *t;
	public:
		typedef intptr_t type;
		static inline type id() {return (type)&ClassID::id;}
	};

	class ClassTraits
	{
	public:
		inline bool have(ClassID<>::type type) const { return _traits.find(type) != _traits.end(); } 
		inline bool set(ClassID<>::type type) { return _traits.insert(type).second; }
	protected:
		std::set<ClassID<>::type> _traits;
	};


	template<typename T>
	class StaticLocker
	{
	public:
		static inline void Lock()
		{
			if (_lockCount == 0)
				T::Locked();
			_lockCount ++;
		}
		static inline void Unlock()
		{
			_lockCount --;
			if (_lockCount == 0)
				T::Unlocked();
		}
		class ScopeLock : public disable_copy_constructors
		{
		public:
			ScopeLock()
			{
				StaticLocker::Lock();
			}
			~ScopeLock()
			{
				StaticLocker::Unlock();
			}
		};
	protected:
		static unsigned _lockCount;
	};

	template <class T> unsigned StaticLocker<T> ::_lockCount = 0;


	template <typename T>
	T lerp_pair(const std::pair<T, T> &pair, T c)
	{
		return pair.first + (pair.second - pair.first) * c;
	}

	template <typename T>
	T mean_pair(const std::pair<T, T> &pair)
	{
		return pair.first + (pair.second - pair.first) / (T)2.0;
	}

	std::wstring stringToWide(const std::string &str);
}


#ifndef UNIQUE_PTR_NONCOPYABLE
#define UNIQUE_PTR_NONCOPYABLE

#include <memory>
#include <type_traits>

template<typename T>
class unique_ptr_copyable
{
public:
	using CopyingFunction = std::unique_ptr<T>(*)(T* p);
	using pointer = T*;

	//constructors
	unique_ptr_copyable() : _copying_function(nullptr)
	{

	}

	unique_ptr_copyable(const unique_ptr_copyable& r)
	{
		copy_from(r);
	}

	unique_ptr_copyable(unique_ptr_copyable&& r)
	{
		swap(r);
	}


	template<class U>
	unique_ptr_copyable(U* r)
	{
		static_assert(std::is_base_of<T, U>::value, "Type mismatch, not a derived type");
		reset(r);
	}

	template<class U>
	unique_ptr_copyable(const unique_ptr_copyable<U>& r)
	{
		copy_from(r);
	}

	template<class U>
	unique_ptr_copyable(unique_ptr_copyable<U>&& r)
	{
		swap(r);
	}


	//copying
	template<class U>
	unique_ptr_copyable& operator=(const unique_ptr_copyable<U>& r)
	{
		copy_from(r);
		return *this;
	}

	unique_ptr_copyable& operator=(nullptr_t)
	{
		reset();
	}

	//moves
	template<class U>
	unique_ptr_copyable& operator=(unique_ptr_copyable<U>&& r)
	{
		swap(r);
		return *this;
	}

	//modifiers
	pointer release()
	{
		_pointer.release();
		_copying_function = nullptr;
	}

	template<typename PT = T>
	void reset(PT* ptr = PT*())
	{
		static_assert(std::is_base_of<T, PT>::value, "Type mismatch, not a derived type");
		_pointer.reset((T*)ptr);
		_copying_function = [](T* p) -> std::unique_ptr<T>
		{
			auto pt = static_cast<PT*>(p);
			T* new_object = new PT(*pt);
			return std::unique_ptr<T>(new_object);
		};
	}


	template<typename U>
	void swap(unique_ptr_copyable<U>& other)
	{
		static_assert(std::is_base_of<T, U>::value, "Type mismatch, not a derived type");
		_pointer.swap(other._pointer);
		std::swap(_copying_function, other._copying_function);
	}

	//observers
	pointer get() const
	{
		return _pointer.get();
	}

	explicit operator bool() const
	{
		return (bool)_pointer;
	}

	//operators
	auto operator*() const
	{
		return *_pointer;
	}

	pointer operator->() const
	{
		return _pointer.get();
	}
protected:
	template<class U>
	void copy_from(const unique_ptr_copyable<U>& r)
	{
		static_assert(std::is_base_of<T, U>::value, "Type mismatch, not a derived type");
		_copying_function = r._copying_function;
		_pointer = _copying_function ? _copying_function(r.get()) : nullptr;

	}

	std::unique_ptr<T> _pointer;
	CopyingFunction    _copying_function;
};

#include "Script/ScriptObject.h"

namespace MX
{
	template<typename T>
	struct PropertyLoader<unique_ptr_copyable<T>>
	{
		using type = PropertyLoader_Standard;
		static bool load(unique_ptr_copyable<T>& out, const Scriptable::Value::pointer& obj)
		{
			auto ret = obj->to_raw_object<T>();
			if (!ret)
				return false;
			assert(false);
			out.reset(ret);
			return true;
		}
	};
}

#endif

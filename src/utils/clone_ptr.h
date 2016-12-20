#pragma once
#include <memory>

namespace MX
{
	struct clone_ptr_default
	{
		template<typename T>
		static auto clone( T&& ptr ) { return ptr ? ptr->clone() : nullptr; }
	};

	template<typename T, typename Strategy = clone_ptr_default>
	class clone_ptr
	{
	public:
		clone_ptr() {}
		clone_ptr( std::nullptr_t other ) {}
		clone_ptr( const clone_ptr& other ) { if ( other ) *this = Strategy::clone(other); }
		clone_ptr( clone_ptr&& other ) : _ptr(other._ptr)  {}
		clone_ptr( const std::shared_ptr<T>& other ) : clone_ptr(Strategy::clone(other))  {}
		clone_ptr( std::shared_ptr<T>&& other ) : _ptr(other)  {}

		operator bool() const { return (bool)_ptr; }
		auto operator->() { return _ptr.operator->(); }
		auto operator->() const { return _ptr.operator->(); }

		clone_ptr& operator = ( const std::shared_ptr<T>& other )
		{
			*this = Strategy::clone(other);
			return *this;
		}

		clone_ptr& operator = ( std::shared_ptr<T>&& other )
		{
			_ptr = other;
			return *this;
		}

		clone_ptr& operator = ( const clone_ptr& other )
		{
			*this = Strategy::clone(other);
			return *this;
		}

		clone_ptr& operator = ( clone_ptr&& other )
		{
			_ptr = other._ptr;
			return *this;
		}

		clone_ptr& operator = ( const std::nullptr_t& other )
		{
			_ptr = nullptr;
			return *this;
		}


		auto get() { return _ptr.get(); }
		const auto& underlying_shared() { return _ptr; }
	protected:
		std::shared_ptr<T> _ptr;
	};
}
#pragma once

#include<memory>
#include<typeindex>

namespace MX
{
	template<typename T=void>
	class ClassID 
	{
		T *t;
	public:
		using type = std::type_index;
		constexpr static inline type id() {return std::type_index(typeid(T));}
	};
}
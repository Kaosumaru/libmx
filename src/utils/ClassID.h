#pragma once

#include<memory>

namespace MX
{
	template<typename T=void>
	class ClassID 
	{
		T *t;
	public:
		using type = intptr_t;
		static inline type id() {return (type)&ClassID::id;}
	};
}
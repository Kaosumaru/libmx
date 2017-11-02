#pragma once
#include <string>
#include "script/serialization/Node.h"

namespace MX
{
	namespace Serialization
	{
		template<typename T>
		struct NodeLoader
		{
			static void sync(T& v, Node&& n)
			{
				static_assert(false, "Loader not implemented");
			}
		};

		template<typename T>
		inline void operator & (T& v, Node&& n)
		{
			NodeLoader<T>::sync(v, std::move(n));
		}

		template<typename X, glm::precision P>
		inline void operator & (glm::tvec2<X, P>& v, Node&& n)
		{
			v.x & n(0);
			v.y & n(1);
		};

	}
}

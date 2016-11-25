#pragma once

#include<memory>
#include<map>
#include<vector>

namespace MX
{
	template<typename T1, typename T2, typename Data>
	class Bimap 
	{
	public:
		using Map = std::map<std::pair<T1, T2>, Data>;

		struct Element
		{
			std::pair<T1, T2> key;
			Data value;

			bool operator < (const Element &e)
			{
				return key < e.key;
			}
		};

		auto insert(T1 t1, T2 t2)
		{
			return _elements.insert(std::make_pair(std::pair<T1,T2>{ t1, t2 }, nullptr)).first;
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
			return _elements.find({ t1, t2 });
		}

		auto erase(typename Map::iterator it)
		{
			return _elements.erase(it);
		}

		template<typename C>
		void enumerate_erase(T1 value, C callback)
		{

		}
	protected:
		Map _elements;
	};
}


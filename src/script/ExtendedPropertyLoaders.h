#pragma once
#include "PropertyLoaders.h"
#include "utils/Random.h"

#if WIP
#include "Graphic/Fonts/Font.h"
#endif
#include <array>

namespace MX
{
	template<typename T>
	struct PropertyLoader<RandomItem<T>>
	{
		using type = PropertyLoader_Standard;
		static bool load(RandomItem<T>& out, const Scriptable::Value::pointer& obj)
		{
			auto &items = obj->array();
			for (auto &item : items)
			{
				if (item->size() == 2)
				{
					auto item_object = (*item)[0].to_object<T>();
					out.AddItem(item_object, (*item)[1]);
					continue;
				}

				auto item_object = item->to_object<T>();
				out.AddItem(item_object, 1.0f);
			}
			return true;
		}
	};

}


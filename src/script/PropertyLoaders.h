#pragma once
#include "Utils/Time.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "ScriptObject.h"
#include "graphic/Color.h"
#include <sstream>
#include <utility>
#include "Utils/SignalizingVariable.h"
#if WIP
#include "Script/Class/MXScriptSoundClass.h"
#include "Script/Class/MXScriptImageClass.h"
#include "Script/Class/MXScriptAnimationClass.h"

#include "Graphic/MXBlender.h"
#include "Graphic/Fonts/MXFont.h"
#endif
#include <array>

namespace MX
{


	template<>
	struct PropertyLoader<Scriptable::Value::pointer>
	{
		using type = PropertyLoader_Standard;
		static bool load(Scriptable::Value::pointer& out, const Scriptable::Value::pointer& obj)
		{
			out = obj;
			return true;
		}
	};

	template<>
	struct PropertyLoader<MX::Color>
	{
		using type = PropertyLoader_Standard;
		static bool load(MX::Color& out, const Scriptable::Value::pointer& obj)
		{
			if (obj->size() < 3)
			{
				auto text = obj->atext();
				if ((text.size() == 7 || text.size() == 9) && text[0] == '#')
				{
					int r, g, b, a = 255;

					// ... and extract the rgb values.
					std::istringstream(text.substr(1, 2)) >> std::hex >> r;
					std::istringstream(text.substr(3, 2)) >> std::hex >> g;
					std::istringstream(text.substr(5, 2)) >> std::hex >> b;
					if (text.size() == 9)
						std::istringstream(text.substr(7, 2)) >> std::hex >> a;

					out = MX::Color((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
					return true;
				}

				out = MX::Color(0xFFFFFFFF);
				return true;
			}

	
			if (obj->size() < 4)
			{
				out = MX::Color(obj->at(0), obj->at(1), obj->at(2), 1.0f);
				return true;
			}
			out = MX::Color(obj->at(0), obj->at(1), obj->at(2), obj->at(3));
			return true;
		}
	};

	template<>
	struct PropertyLoader<MX::ColorExtended>
	{
		using type = PropertyLoader_Standard;
		static bool load(MX::ColorExtended& out, const Scriptable::Value::pointer& obj)
		{
			MX::Color color;
			PropertyLoader<MX::Color>::load(color, obj);
			out = color;
			return true;
		}
	};






	template<>
	struct PropertyLoader<bool>
	{
		using type = PropertyLoader_Standard;
		static bool load(bool& out, const Scriptable::Value::pointer& obj)
		{
			out = obj->isTrue();
			return true;
		}
	};

	template<typename T>
	struct PropertyLoader<std::shared_ptr<T>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::shared_ptr<T>& out, const Scriptable::Value::pointer& obj)
		{
			auto ret = obj->to_object<T>();
			if (!ret)
				return false;
			out = ret;
			return true;
		}
	};



	template<typename T>
	struct PropertyLoader<std::unique_ptr<T>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::unique_ptr<T>& out, const Scriptable::Value::pointer& obj)
		{
			auto ret = obj->to_raw_object<T>();
			if (!ret)
				return false;
			assert(false);
			out.reset(ret);
			return true;
		}
	};


	template<typename T>
	struct PropertyLoader<SignalizingVariable<T>>
	{
		using type = PropertyLoader_Standard;
		static bool load(SignalizingVariable<T>& out, const Scriptable::Value::pointer& obj)
		{
			T t;
			if (PropertyLoader<T>::load(t, obj))
			{
				out = t;
				return true;
			}

			return false;
		}
	};

    template<typename T, unsigned size>
    struct PropertyLoader<std::array<T, size>>
    {
        using type = PropertyLoader_Standard;
        static bool load(std::array<T, size>& out, const Scriptable::Value::pointer& obj)
        {
            if (obj->array().size() == 0)
            {
                out.fill(T{});
                return true;
            }
            if (obj->array().size() != size)
                return false;

            auto it = out.begin();
            for (auto &i : obj->array())
            {
                PropertyLoader<T>::load(*it, i);
                ++it;
            }

            return true;
        }
    };

	template<typename T>
	struct PropertyLoader<std::list<T>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::list<T>& out, const Scriptable::Value::pointer& obj)
		{
			out.clear();
			if (obj->array().size() == 0)
				return true;
			out.resize(obj->array().size());

			auto it = out.begin();
			for (auto &i : obj->array())
			{
				PropertyLoader<T>::load(*it, i);
				++it;
			}

			return true;
		}
	};

	template<typename T>
	struct PropertyLoader<std::vector<T>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::vector<T>& out, const Scriptable::Value::pointer& obj)
		{
			out.clear();
			if (obj->array().size() == 0)
				return true;
			out.resize(obj->array().size());

			auto it = out.begin();
			for (auto &i : obj->array())
			{
				PropertyLoader<T>::load(*it, i);
				++it;
			}

			return true;
		}
	};

	template<typename T>
	struct PropertyLoader<std::set<T>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::set<T>& out, const Scriptable::Value::pointer& obj)
		{
			for (auto &i : obj->array())
			{
				T t;
				PropertyLoader<T>::load(t, i);
				out.insert(t);
			}
				
			return true;
		}
	};

	template<typename T1, typename T2>
	struct PropertyLoader<std::map<T1, T2>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::map<T1, T2>& out, const Scriptable::Value::pointer& obj)
		{
			for (auto &item : obj->array())
			{
				if (item->size() < 2)
					continue;
				auto &i = *item;

				T1 t1;
				T2 t2;

				PropertyLoader<T1>::load(t1, i.array()[0]);
				PropertyLoader<T2>::load(t2, i.array()[1]);

				out.emplace(std::move(t1), std::move(t2));
			}
			return true;
		}
	};

	template<typename T2>
	struct PropertyLoader<std::map<std::string, T2>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::map<std::string, T2>& out, const Scriptable::Value::pointer& obj)
		{
			for (auto &p : obj->map())
			{
				T2 t2;
				PropertyLoader<T2>::load(t2, p.second);
				out.emplace(p.first, std::move(t2));
			}

			for (auto &item : obj->array())
			{
				if (item->size() < 2)
					continue;
				auto &i = *item;

				std::string t1;
				T2 t2;

				PropertyLoader<std::string>::load(t1, i.array()[0]);
				PropertyLoader<T2>::load(t2, i.array()[1]);

				out.emplace(std::move(t1), std::move(t2));
			}
			return true;
		}
	};


	template<typename T1, typename T2>
	struct PropertyLoader<std::pair<T1, T2>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::pair<T1, T2>& out, const Scriptable::Value::pointer& obj)
		{
			if (obj->size() == 2)
			{
				PropertyLoader<T1>::load(out.first, obj->array()[0]);
				PropertyLoader<T2>::load(out.second, obj->array()[1]);
			}
			else
			{
				PropertyLoader<T1>::load(out.first, obj);
				PropertyLoader<T2>::load(out.second, obj);
			}
			return true;
		}
	};


	template<typename... T>
	struct PropertyLoader<std::tuple<T...>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::tuple<T...>& out, const Scriptable::Value::pointer& obj)
		{
			auto size = std::tuple_size<typename std::tuple<T...>>::value;
			if (obj->size() != size)
				return false;

			return load_impl(out, obj, std::index_sequence_for<T...>());
		}

		template<typename Tuple, std::size_t... Is>
		static bool load_impl(Tuple &t, const Scriptable::Value::pointer& obj, std::index_sequence<Is...>)
		{
			bool success = true;
			auto fill_args = {
				0, ((
				
				success &= PropertyLoader<T>::load(std::get<Is>(t), obj->array()[Is])
					
				), 0)...
			};
			return success;
		}
	};

#ifdef WIP
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

	template<typename T>
	struct PropertyLoader<RandomNonLockedItem<T>>
	{
		using type = PropertyLoader_Standard;
		static bool load(RandomNonLockedItem<T>& out, const Scriptable::Value::pointer& obj)
		{
			return PropertyLoader<RandomItem<T>>::load(out, obj);
		}
	};


	
	template<typename T>
	struct PropertyLoader<Time::XPerSecond<T>>
	{
		using type = PropertyLoader_Standard;
		static bool load(Time::XPerSecond<T>& out, const Scriptable::Value::pointer& obj)
		{
			T t;
			PropertyLoader<T>::load(t, obj);
			out = t;
			return true;
		}
	};

	template<>
	struct PropertyLoader<std::shared_ptr<Graphic::Image>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::shared_ptr<Graphic::Image>& out, const Scriptable::Value::pointer& obj)
		{
			auto loader = obj->to_object<ScriptImageClass>();
			if (!loader || !loader->image())
				return false;
			out = loader->image();
			return true;
		}
	};

	template<>
	struct PropertyLoader<std::shared_ptr<Graphic::QuadImage>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::shared_ptr<Graphic::QuadImage>& out, const Scriptable::Value::pointer& obj)
		{
			auto loader = obj->to_object<ScriptSlice9ImageClass>();
			if (!loader || !loader->image())
				return false;
			out = loader->image();
			return true;
		}
	};

	template<>
	struct PropertyLoader<std::shared_ptr<Graphic::Surface>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::shared_ptr<Graphic::Surface>& out, const Scriptable::Value::pointer& obj)
		{
			auto loader = obj->to_object<ScriptImageClass>();
			if (!loader || !loader->image())
				return false;
			auto surface = std::dynamic_pointer_cast<Graphic::Surface>(loader->image());
			if (!surface)
				return false;
			out = surface;
			return true;
		}
	};

	template<>
	struct PropertyLoader<std::shared_ptr<Sound::Sample>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::shared_ptr<Sound::Sample>& out, const Scriptable::Value::pointer& obj)
		{
			auto loader = obj->to_object<ScriptSoundClass>();
			if (!loader || !loader->sound())
				return false;
			out = loader->sound();
			return true;
		}
	};

	template<>
	struct PropertyLoader<std::shared_ptr<Graphic::SingleAnimation>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::shared_ptr<Graphic::SingleAnimation>& out, const Scriptable::Value::pointer& obj)
		{
			auto loader = obj->to_object<ScriptAnimationClass>();
			if (!loader || !loader->animation())
				return false;
			out = loader->animation();
			return true;
		}
	};


	template<>
	struct PropertyLoader<MX::Graphic::Font::pointer>
	{
		using type = PropertyLoader_Custom;
		static bool load(MX::Graphic::Font::pointer& out, const MX::Scriptable::Value &value)
		{
			ScriptObjectString script(value.fullPath());


			std::string name;
			std::string face;
			unsigned size = 10;
			if (script.load_property(name, "Name"))
			{
				script.load_property(size, "Size");
				script.load_property(face, "Face");
				out = MX::Graphic::Font::Create(name, size, face);
				return true;
			}
			return false;
		}
	};
	
	
	template<>
	struct PropertyLoader<MX::Graphic::Blender>
	{
		using type = PropertyLoader_Custom;
		static bool load(MX::Graphic::Blender& out, const MX::Scriptable::Value &value)
		{
			ScriptObjectString script(value.fullPath());

			auto stringToOp = [](const std::string &op)
			{
				if (op == "Add")
					return GL_FUNC_ADD;
				if (op == "SrcMinusDest")
					return GL_FUNC_SUBTRACT;
				if (op == "DestMinusSrc")
					return GL_FUNC_REVERSE_SUBTRACT;
				return GL_FUNC_ADD;
			};

			auto stringToSrc = [](const std::string &op)
			{


				if (op == "Zero")
					return GL_ZERO;
				if (op == "One")
					return GL_ONE;
				if (op == "Alpha")
					return GL_SRC_ALPHA;
				if (op == "InverseAlpha")
					return GL_ONE_MINUS_SRC_ALPHA;
				if (op == "SrcColor")
					return GL_SRC_COLOR;
				if (op == "DestColor")
					return GL_DST_COLOR;
				if (op == "InverseSrcColor")
					return GL_ONE_MINUS_SRC_COLOR;
				if (op == "InverseDestColor")
					return GL_ONE_MINUS_DST_COLOR;
				return GL_ZERO;
			};


			std::string op;
			if (script.load_property(op, "Op"))
			{
				std::string src, dest;
				script.load_property(src, "Src");
				script.load_property(dest, "Dest");

				out = { stringToOp(op), stringToSrc(src), stringToSrc(dest) };
				return true;
			}
			return false;
		}
	};
#endif



	template<>
	struct PropertyLoader<glm::vec2>
	{
		using type = PropertyLoader_Standard;
		static bool load(glm::vec2& out, const Scriptable::Value::pointer& obj)
		{
			if (obj->size() == 0)
			{
				float x = *obj;
				out.x = out.y = x;
			}
			else if (obj->size() == 2)
			{
				out.x = *(obj->array()[0]);
				out.y = *(obj->array()[1]);
			}
			else return false;

			return true;
		}
	};

	template<>
	struct PropertyLoader<std::pair<glm::vec2, glm::vec2>>
	{
		using type = PropertyLoader_Standard;
		static bool load(std::pair<glm::vec2, glm::vec2>& out, const Scriptable::Value::pointer& obj)
		{
			if (obj->size() == 2 && obj->at(0).size() == 2 && obj->at(1).size() == 2)
			{
				PropertyLoader<glm::vec2>::load(out.first, obj->array()[0]);
				PropertyLoader<glm::vec2>::load(out.second, obj->array()[1]);
			}
			else
			{
				PropertyLoader<glm::vec2>::load(out.first, obj);
				PropertyLoader<glm::vec2>::load(out.second, obj);
			}
			return true;
		}
	};


	template<>
	struct PropertyLoader<glm::vec3>
	{
		using type = PropertyLoader_Standard;
		static bool load(glm::vec3& out, const Scriptable::Value::pointer& obj)
		{
			if (obj->size() == 0)
			{
				float x = *obj;
				out.x = out.y = out.z = x;
			}
			else if (obj->size() == 3)
			{
				out.x = *(obj->array()[0]);
				out.y = *(obj->array()[1]);
				out.z = *(obj->array()[2]);
			}
			else return false;

			return true;
		}
	};

}


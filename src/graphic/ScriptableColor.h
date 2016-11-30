#ifndef MXCOLORSCRIPTABLE
#define MXCOLORSCRIPTABLE
#include "Color.h"
#include "Script/Scriptable/ScriptableValue.h"
#include "Script/PropertyLoaders.h"

namespace MX{

class ScriptObject;

class ScriptableColor
{
public:
	using ColorsValue = std::array<Scriptable::Value::pointer, 4>;

	ScriptableColor() {}
	ScriptableColor(const ColorsValue& values) { SetValues(values); }

	operator const Color&()
	{ 
		if (!isConstant())
			Recalculate();
		return _color;
	}

	void SetValue(const Scriptable::Value::pointer& value);
	void SetValues(const ColorsValue& values);
	bool isConstant() { return _constant; }
protected:
	
	void Recalculate();

	Scriptable::Value::pointer _value;
	ColorsValue _channels;
	bool _constant = true;
	Color _color;
};


template<>
struct PropertyLoader<MX::ScriptableColor>
{
	using type = PropertyLoader_Standard;
	static bool load(MX::ScriptableColor& out, const Scriptable::Value::pointer& obj)
	{
		if (obj->size() == 0)
		{
			out.SetValue(obj);
			return true;
		}


		if (obj->size() == 3)
		{
			out.SetValues({ obj->array()[0], obj->array()[1], obj->array()[2], nullptr });
			return true;
		}

		if (obj->size() == 4)
		{
			out.SetValues({ obj->array()[0], obj->array()[1], obj->array()[2], obj->array()[3]});
			return true;
		}
		
		return false;
	}
};


}

#endif

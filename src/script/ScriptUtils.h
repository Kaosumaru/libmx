#pragma once

#include "Script.h"
#include "script/PropertyLoaders.h"


namespace MX
{

	template<typename XType>
	struct XOrExpression
	{
	public:
		XOrExpression(XType value = {}) : _value(value)
		{

		}

		operator const XType&() const
		{
			if (_expr)
				const_cast<XOrExpression*>(this)->_value = *_expr;
			return _value;
		}

		void SetValue(XType value = 0.0f)
		{
			_expr.reset();
			_value = value;
		}

		void SetExpression(const Scriptable::Value::pointer& expr)
		{
			_expr = expr;
		}
	protected:
		Scriptable::Value::pointer _expr;
		XType                      _value;
	};

	using FloatOrExpression = XOrExpression<float>;
	using IntOrExpression = XOrExpression<int>;

	template<typename XType>
	struct PropertyLoader<MX::XOrExpression<XType>>
	{
		using type = PropertyLoader_Standard;
		static bool load(MX::XOrExpression<XType>& out, const Scriptable::Value::pointer& obj)
		{
			if (!obj->member()->isConstant())
				out.SetExpression(obj);
			else
			{
				XType val;
				if (!PropertyLoader<XType>::load(val, obj))
					return false;
				out.SetValue(val);
			}
			return true;
		}
	};

}
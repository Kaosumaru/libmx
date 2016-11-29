#pragma once
#include<memory>
#include<map>
#include<string>
#include "utils/Vector2.h"

namespace MX
{
namespace Easing
{

using Function = float(*)(float, float, float);

Function EasingFromString(const std::string& easing);

template<typename T>
T linear(T b, T c, T t)
{
	return b + c * t;
}


template < typename Base, typename T >
struct base_easing
{
	static T in_out(T b, T c, T t)
	{
		auto half_c = c * 0.5f;
		if (t < 0.5)
			return Base::in(b, half_c, t * 2);
		return Base::out(b + half_c, half_c, t * 2.0f - 1.0f);
	}
};


template<typename T>
struct quad : public base_easing<quad<T>, T>
{
	static T in(T b, T c, T t)
	{
		return b + c * t * t;
	}

	static T out(T b, T c, T t)
	{
		return b + -c * (t - 2) * t;
	}
};

template<typename T>
struct cubic : public base_easing< cubic<T>, T>
{
	static T in(T b, T c, T t)
	{
		return b + c * t * t * t;
	}

	static T out(T b, T c, T t)
	{
		t -= 1;
		return b + c * (t*t*t + 1);
	}
};

template<typename T>
struct quartic : public base_easing< quartic<T>, T>
{
	static T in(T b, T c, T t)
	{
		return b + c * t * t * t * t;
	}
	static T out(T b, T c, T t)
	{
		t -= 1;
		return b - c * (t*t*t*t - 1);
	}
};




template<typename T>
struct quintic : public base_easing< quintic<T>, T>
{
	static T in(T b, T c, T t)
	{
		return b + c * t * t * t * t * t;
	}

	static T out(T b, T c, T t)
	{
		t -= 1;
		return c*(t*t*t*t*t + 1) + b;
	}
};

template<typename T>
struct sinusoidal
{
	static T in(T b, T c, T t)
	{
		return b + c - c * cos(t * (float)MX_PI / 2.0f);
	}

	static T out(T b, T c, T t)
	{
		return b + c * sin(t * (float)MX_PI / 2.0f);
	}

	static T in_out(T b, T c, T t)
	{
		return -c / 2.0f * (cos(MX_PI*t) - 1.0f) + b;
	}
};

template<typename T>
struct exponential : public base_easing< exponential<T>, T>
{
	static T in(T b, T c, T t)
	{
		return b + c * pow(2, 10 * (t - 1));
	}

	static T out(T b, T c, T t)
	{
		return b + c * (-pow(2, -10 * t) + 1.0f);
	}

};

template<typename T>
struct circular : public base_easing< circular<T>, T>
{
	static T in(T b, T c, T t)
	{
		return b - c * (sqrt(1.0f - t*t) - 1);
	}

	static T out(T b, T c, T t)
	{
		t -= 1;
		return b + c * sqrt(1.0f - t*t);
	}

};



}
}

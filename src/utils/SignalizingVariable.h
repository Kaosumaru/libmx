#pragma once
#include<memory>
#include "Signal.h"


template <typename T, template<class> class Parent>
class SignalizingVariableBase : public Parent<T>
{
public:
	SignalizingVariableBase() {}
	SignalizingVariableBase(const T& t) :_t(t) {}
	SignalizingVariableBase(const SignalizingVariableBase& x)
	{
		_t = x._t;
	}


	SignalizingVariableBase& operator=(const T &x)
    {
        T old = _t;
        _t = x;
		__onChangedImpl(_t, old);
        return *this;
    }
    
	SignalizingVariableBase& operator=(const SignalizingVariableBase &x)
	{ 
		return (*this = x._t);
	}


	operator bool () const
	{
		return _t != 0;
	}

    operator T& ()
    {
        return _t;
    }

	operator const T& () const
	{
		return _t;
	}

	T& operator --()
	{
		T old = _t;
        --_t ;
		__onChangedImpl(_t, old);
		return _t;
	}

	T operator --(int)
	{
		T old = _t;
        --_t ;
		__onChangedImpl(_t, old);
		return old;
	}

	T& operator ++()
	{
		T old = _t;
        ++_t ;
		__onChangedImpl(_t, old);
		return _t;
	}

	T& operator -=(const T& other)
	{
		T old = _t;
		_t -= other;
		__onChangedImpl(_t, old);
		return _t;
	}

	T& operator +=(const T& other)
	{
		T old = _t;
		_t += other;
		__onChangedImpl(_t, old);
		return _t;
	}

	T& operator *=(const T& other)
	{
		T old = _t;
		_t *= other;
		__onChangedImpl(_t, old);
		return _t;
	}

	T& operator /=(const T& other)
	{
		T old = _t;
		_t /= other;
		__onChangedImpl(_t, old);
		return _t;
	}

	T operator ++(int)
	{
		T old = _t;
        ++_t ;
		__onChangedImpl(_t, old);
		return old;
	}

	T& directValueAccess() { return _t; }

	auto operator -> ()
	{
		return _t.operator->();
	}

	MX::Signal<void(const T& newValue, const T& oldValue)> onValueChanged;
protected:
	void __onChangedImpl(const T& newValue, const T& oldValue)
	{
		onValueChanged(newValue, oldValue);
		Parent<T>::_onChanged(newValue, oldValue);
	}

	T _t;
};


template <typename T>
class SignalizingVariable_Impl
{
protected:
	void _onChanged(const T& newValue, const T& oldValue)
	{
		
	}
};

template<typename T>
using SignalizingVariable = SignalizingVariableBase<T, SignalizingVariable_Impl>;



template <typename T>
class SignalizingVariableTimed_Impl
{
public:
	SignalizingVariableTimed_Impl()
	{

	}

	float timeOfChange() { return _timeOfChange; }
protected:
	void _onChanged(const T& newValue, const T& oldValue)
	{
		_timeOfChange = Time::Timer::current().total_seconds();
	}
	float _timeOfChange = 0.0f;
};

template<typename T>
using SignalizingVariableTimed = SignalizingVariableBase<T, SignalizingVariableTimed_Impl>;


template <typename T>
class ModificableVariable
{
public:
    ModificableVariable() :_t(0) , _access(0) {}
    ModificableVariable(const T& t) :_t(t) , _access(t) {}
    ModificableVariable(const ModificableVariable& x)
	{
		_access = _t = x._t;
	}

    ModificableVariable& operator=(const T &x)
    {
		onValueChanged(x, _t);
        _access = _t = x;
        return *this;
    }
    

	ModificableVariable& operator=(const ModificableVariable &x){ onValueChanged(x._t, _t); _t = x._t; return *this; }

	ModificableVariable& operator*=(const T &x){ _t *= x; return *this; } //TODO signalize
	ModificableVariable& operator/=(const T &x){ _t /= x; return *this; } //TODO signalize
	ModificableVariable& operator+=(const T &x){ _t += x; return *this; } //TODO signalize
	ModificableVariable& operator-=(const T &x){ _t -= x; return *this; }

	const T& get()
	{
		_access = _t;
		onAccessingValue(_access);
		return _access;
	}

    operator T& ()
    {
        return _access;
    }    

	typedef MX::SimpleSignal<T&> AccessingValueSignal;
	typedef MX::SimpleSignal<const T&, const T&> ValueChangedSignal; //new, old



    ValueChangedSignal onValueChanged;
	AccessingValueSignal onAccessingValue;
protected:

	T _access;
	T _t;
};



template<typename T>
class Modifier
{
public:
    Modifier() : base(0), multiplier(1.0f) {}
    Modifier(const T& t) : multiplier(1.0f), base(t) {}
    Modifier(const Modifier& x)
	{
		base = x.base;
		multiplier = x.multiplier;
	}

    Modifier& operator=(const T &x)
    {
        base = x;
        return *this;
    }
    
	Modifier& operator=(const Modifier &x)
	{  
		base = x.base;
		multiplier = x.multiplier;
		return *this; 
	}

	const T get()
	{
		return (T)(base.get() * multiplier.get());
	}

	const T get(const T& v)
	{
		return (T)((v + base.get()) * multiplier.get());
	}

    operator T ()
    {
        return (T)(base * multiplier);
    }    

	ModificableVariable<T> base;
	ModificableVariable<float> multiplier;
};


#endif

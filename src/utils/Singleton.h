#pragma once
#include "utils/Signal.h"
#include <memory>
#include <set>

template <typename T, typename T1 = void>
class ScopeSingleton
{
public:
    class ContextGuard
    {
    public:
        ContextGuard() { }
        ContextGuard(ContextGuard&) = delete;
        ContextGuard(ContextGuard&& other)
        {
            std::swap(_old, other._old);
            std::swap(_hasValue, other._hasValue);
        }

        ContextGuard(T& _target)
            : _old(&(ScopeSingleton<T, T1>::current()))
            , _hasValue(true)
        {
            Reset(_target);
        }
        ~ContextGuard()
        {
            if (_hasValue)
                ScopeSingleton<T, T1>::SetCurrent(*_old);
        }

        void Reset(T& _target)
        {
            if (!_hasValue)
            {
                _old = &(ScopeSingleton<T, T1>::current());
                _hasValue = true;
            }

            ScopeSingleton<T, T1>::SetCurrent(_target);
        }

    protected:
        T* _old = nullptr;
        bool _hasValue = false;
    };

    using ContextGuardPtr = std::unique_ptr<ContextGuard>;

    static auto CreateGuard(T& _target) { return std::make_unique<ContextGuard>(_target); }

    static void SetCurrent(T& _target)
    {
        _current_static_singleton = &_target;
    }

    static void SetCurrent(T* _target)
    {
        _current_static_singleton = _target;
    }

    static void SetCurrent(const std::shared_ptr<T>& _target)
    {
        _current_static_singleton = _target.get();
    }

    static T& current()
    {
        return *_current_static_singleton;
    }

    static T* current_pointer()
    {
        return _current_static_singleton;
    }

    static bool isCurrent()
    {
        return _current_static_singleton != 0;
    }

    ContextGuard Use()
    {
        return ContextGuard((T&)(*this));
    }

protected:
    static T* _current_static_singleton;
};

template <class T, typename T1>
T* ScopeSingleton<T, T1>::_current_static_singleton = nullptr;

template <typename T, typename T1 = void>
struct Context : public ScopeSingleton<T, T1>
{
    static typename ScopeSingleton<T, T1>::ContextGuard CreateEmptyGuard()
    {
        return typename ScopeSingleton<T, T1>::ContextGuard();
    }

    static void Lock(T&& t)
    {
    }

    static typename ScopeSingleton<T, T1>::ContextGuard Lock(T& t)
    {
        return typename ScopeSingleton<T, T1>::ContextGuard((T&)t);
    }

    static typename ScopeSingleton<T, T1>::ContextGuard Lock(T* t)
    {
        return typename ScopeSingleton<T, T1>::ContextGuard((T&)(*t));
    }

    static typename ScopeSingleton<T, T1>::ContextGuard Lock(const std::shared_ptr<T>& t)
    {
        return typename ScopeSingleton<T, T1>::ContextGuard((T&)*(t.get()));
    }
};

template <typename T>
class ScopeSingletonDefault
{
public:
    class Context
    {
    public:
        Context(T& _target)
            : _old(ScopeSingletonDefault<T>::current())
        {
            ScopeSingletonDefault<T>::SetCurrent(_target);
        }
        ~Context() { ScopeSingletonDefault<T>::SetCurrent(_old); }

    protected:
        T& _old;
    };

    static void SetCurrent(T& _target)
    {
        _current_static_singleton = &_target;
    }

    static T& current()
    {
        if (_current_static_singleton == nullptr)
        {
            static T t;
            return t;
        }
        return *_current_static_singleton;
    }

    static T* pointer_to_current()
    {
        return _current_static_singleton;
    }

    Context Use()
    {
        return Context((T&)(*this));
    }

protected:
    static T* _current_static_singleton;
};

template <class T>
T* ScopeSingletonDefault<T>::_current_static_singleton = nullptr;

template <typename T>
class ScopeSingletonFunction
{
public:
    class Context
    {
    public:
        Context(T& _target)
            : _old(T::current())
        {
            T::SetCurrent(_target);
        }
        ~Context() { T::SetCurrent(_old); }

    protected:
        T& _old;
    };

    static void SetCurrent(T& _target)
    {
        auto old = _current_static_singleton;
        _current_static_singleton = &_target;
        T::CurrentWasSet(_current_static_singleton, old);
    }

    static T& current()
    {
        return *_current_static_singleton;
    }

    Context Use()
    {
        return Context((T&)(*this));
    }

protected:
    static T* _current_static_singleton;
};

template <class T>
T* ScopeSingletonFunction<T>::_current_static_singleton = nullptr;

template <typename T>
class Singleton
{
public:
    static T& get()
    {
        static T t;
        return t;
    }
};

class MainDeinitializer : public Singleton<MainDeinitializer>
{
public:
    static auto& onDeinit() { return get().deinit; }

    void RunDeinit()
    {
        deinit();
        deinit.disconnect_all_slots();
    }

    MX::Signal<void(void)> deinit;
};

template <typename T>
class DeinitSingleton
{
    static std::unique_ptr<T> t;

public:
    static T& get()
    {

        static bool _initialized = false;
        if (!_initialized)
        {
            _initialized = true;
            t.reset(new T);
            MainDeinitializer::onDeinit().static_connect([&]() { Deinit(); });
        }

        return *t;
    }

    static void Deinit()
    {
        t.reset();
    }
};

template <typename T>
std::unique_ptr<T> DeinitSingleton<T>::t;
#ifndef MXCONTEXTSTACK
#define MXCONTEXTSTACK
#include <memory>
#include <set>

template <typename T, typename T1 = void>
class ContextStack
{
public:
    class ContextGuard
    {
    public:
        using Parent = ScopeSingleton<T, T1>;

        ContextGuard(T& target)
            : _pointer(target)
        {
            Parent::PushCurrent(_pointer);
        }
        ~ContextGuard()
        {
            Parent::PopCurrent(_pointer);
        }

    protected:
        T* _pointer;
    };

    friend class ContextGuard;

    using ContextGuardPtr = std::unique_ptr<ContextGuard>;

    static auto Lock(T& _target) { return std::make_unique<ContextGuard>(_target); }

    static const std::vector<T*>& stack()
    {
        return _stack;
    }

    ContextGuard Use()
    {
        return ContextGuard((T&)(*this));
    }

    static void PushCurrent(T* _target)
    {
        _stack.push_back(_target);
    }

    static void PopCurrent(T* _target)
    {
        assert(_stack.back() == _target);
        _stack.pop_back();
    }

protected:
    static std::vector<T*> _stack;
};

template <typename T, typename T1>
std::vector<T*> ContextStack<T, T1>::_stack;

#endif

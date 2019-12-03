#ifndef POOLED_OBJECT
#define POOLED_OBJECT

#include "MemoryPool.h"

template <typename T, size_t BlockSize = 4096>
class PooledObject : public T
{
public:
    using T::T;

    static void* operator new(std::size_t sz)
    {
        return _mpool.allocate();
    }

    static void operator delete(void* ptr)
    {
        _mpool.deallocate((T*)ptr);
    }

protected:
    static MemoryPool<T, BlockSize> _mpool;
};

template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize> PooledObject<T, BlockSize>::_mpool;

#endif // MEMORY_POOL_H

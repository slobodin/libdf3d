#pragma once

#include <type_traits>

namespace df3d {

class Allocator : NonCopyable
{
public:
    static const size_t DEFAULT_ALIGN = 4;

    Allocator() = default;
    virtual ~Allocator() = default;

    virtual void* alloc(size_t size, size_t alignment = DEFAULT_ALIGN) = 0;
    virtual void dealloc(void *mem) = 0;

    template<typename T, typename ...Args>
    T* makeNew(Args &&...args)
    {
        return new (alloc(sizeof(T), alignof(T))) T(std::forward<Args>(args)...);
    }

    template<class T>
    void makeDelete(T *obj)
    {
        if (!obj)
            return;
        obj->~T();
        dealloc(obj);
    }
};

}

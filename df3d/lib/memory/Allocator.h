#pragma once

namespace df3d {

class Allocator : NonCopyable
{
public:
    static const size_t DEFAULT_ALIGN = 4;

    Allocator() = default;
    virtual ~Allocator() = default;

    virtual void* alloc(size_t size, size_t alignment) = 0;
    virtual void dealloc(void *mem) = 0;
    virtual size_t bytesAllocated() = 0;
};

template<typename T, typename Alloc>
static void DeleteHelper(Alloc &a, T *obj)
{
    if (!obj)
        return;
    obj->~T();
    a.dealloc(obj);
}

#define MAKE_NEW(allocator, T) new ((allocator).alloc(sizeof(T), alignof(T))) T
#define MAKE_DELETE(allocator, obj) DeleteHelper(allocator, obj)

#define MEM_ALLOC(allocator, T, count) (T*)((allocator).alloc(sizeof(T) * count, alignof(T)))
#define MEM_FREE(allocator, mem) (allocator).dealloc(mem)

}

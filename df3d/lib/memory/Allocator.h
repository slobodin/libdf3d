#pragma once

namespace df3d {

class DF3D_DLL Allocator : NonCopyable
{
public:
    static const size_t DEFAULT_ALIGN = 4;

    Allocator() = default;
    virtual ~Allocator() = default;

    virtual void* alloc(size_t size, size_t alignment = DEFAULT_ALIGN) = 0;
    virtual void dealloc(void *mem) = 0;
};

}

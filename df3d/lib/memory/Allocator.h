#pragma once

namespace df3d {

class DF3D_DLL Allocator : NonCopyable
{
public:
    Allocator() = default;
    virtual ~Allocator() = default;

    virtual void* allocate(size_t size) = 0;
    virtual void* deallocate(void *mem) = 0;
};

}

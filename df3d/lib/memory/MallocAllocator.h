#pragma once

#include "Allocator.h"

namespace df3d {

class DF3D_DLL MallocAllocator : public Allocator
{
    int m_totalAllocated = 0;

public:
    MallocAllocator();
    ~MallocAllocator();

    void* alloc(size_t size, size_t alignment = DEFAULT_ALIGN) override;
    void dealloc(void *mem) override;
};

}

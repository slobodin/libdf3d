#pragma once

#include "Allocator.h"

namespace df3d {

class MallocAllocator : public Allocator
{
    int m_totalAllocated = 0;
    std::mutex m_lock;

public:
    MallocAllocator();
    ~MallocAllocator();

    void* alloc(size_t size, size_t alignment) override;
    void dealloc(void *mem) override;
    size_t bytesAllocated() override;
};

}

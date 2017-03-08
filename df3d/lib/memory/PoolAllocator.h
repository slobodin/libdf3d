#pragma once

#include "Allocator.h"

namespace df3d {

class PoolAllocator : public Allocator
{
    Allocator &m_alloc;

    void *m_pool;
    size_t m_itemSize;
    size_t m_maxItems;
    size_t m_align;
    void *m_freeList;
    int m_totalAllocated;
    size_t m_bytesAllocated;

public:
    PoolAllocator(Allocator &alloc, size_t maxItems, size_t itemSize, size_t align);
    ~PoolAllocator();

    void* alloc(size_t size, size_t alignment) override;
    void dealloc(void *mem) override;
    size_t bytesAllocated() override;
};

}

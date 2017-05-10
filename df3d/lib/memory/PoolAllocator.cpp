#include "PoolAllocator.h"

namespace df3d {

PoolAllocator::PoolAllocator(Allocator &alloc, size_t maxItems, size_t itemSize, size_t align)
    : m_alloc(alloc),
    m_itemSize(itemSize),
    m_maxItems(maxItems),
    m_align(align)
{
    DF3D_ASSERT(itemSize >= sizeof(uintptr_t));

    auto blockSize = itemSize + align;

    m_bytesAllocated = blockSize * (m_maxItems + 1);
    m_pool = alloc.alloc(m_bytesAllocated, align);

    auto curr = (uint8_t*)m_pool;
    for (size_t i = 0; i < m_maxItems; i++)
    {
        *((uintptr_t*)curr) = uintptr_t(curr + blockSize);
        curr += blockSize;
    }

    // Set last item to NULL.
    *((uintptr_t*)curr) = 0;
    m_freeList = m_pool;
}

PoolAllocator::~PoolAllocator()
{
    m_alloc.dealloc(m_pool);
}

void* PoolAllocator::alloc(size_t size, size_t alignment)
{
    DF3D_ASSERT(size == m_itemSize);
    DF3D_ASSERT(alignment == m_align);
    if (m_freeList == nullptr)
    {
        DF3D_ASSERT_MESS(false, "Memory pool is out of memory");
        return nullptr;
    }

    auto nextFree = *((uintptr_t*)m_freeList);
    auto retRes = m_freeList;
    m_freeList = (void*)nextFree;

    m_totalAllocated++;

    return retRes;
}

void PoolAllocator::dealloc(void *mem)
{
    if (!mem)
        return;

    DF3D_ASSERT(m_totalAllocated > 0);

    (*(uintptr_t*)mem) = (uintptr_t)m_freeList;
    m_freeList = mem;

    m_totalAllocated--;
}

size_t PoolAllocator::bytesAllocated()
{
    return m_bytesAllocated;
}

}

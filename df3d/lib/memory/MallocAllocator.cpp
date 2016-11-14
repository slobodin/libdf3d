#include "MallocAllocator.h"

#include <df3d/lib/Utils.h>

namespace df3d {

MallocAllocator::MallocAllocator()
{

}

MallocAllocator::~MallocAllocator()
{
    DF3D_ASSERT(m_totalAllocated == 0);
}

void* MallocAllocator::alloc(size_t size, size_t alignment)
{
    std::lock_guard<std::mutex> lock(m_lock);

    //TODO
    ++m_totalAllocated;
    return _aligned_malloc(size, alignment);
}

void MallocAllocator::dealloc(void *mem)
{
    if (!mem)
        return;

    std::lock_guard<std::mutex> lock(m_lock);

    --m_totalAllocated;
    DF3D_ASSERT(m_totalAllocated >= 0);

    _aligned_free(mem);
}

size_t MallocAllocator::bytesAllocated()
{
    return 0;
}

}

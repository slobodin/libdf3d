#include "MallocAllocator.h"

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
    //TODO
    ++m_totalAllocated;
    return malloc(size);
}

void MallocAllocator::dealloc(void *mem)
{
    if (!mem)
        return;

    --m_totalAllocated;
    DF3D_ASSERT(m_totalAllocated >= 0);

    free(mem);
}

}

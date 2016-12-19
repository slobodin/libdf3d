#include "MallocAllocator.h"

#include <df3d/lib/Utils.h>
#ifndef WIN32
#include <mm_malloc.h>
#endif

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
#ifdef WIN32
    return _aligned_malloc(size, alignment);
#else
    return _mm_malloc(size, alignment);
#endif
}

void MallocAllocator::dealloc(void *mem)
{
    if (!mem)
        return;

    std::lock_guard<std::mutex> lock(m_lock);

    --m_totalAllocated;
    DF3D_ASSERT(m_totalAllocated >= 0);

#ifdef WIN32
    _aligned_free(mem);
#else
    _mm_free(mem);
#endif
}

size_t MallocAllocator::bytesAllocated()
{
    return 0;
}

}

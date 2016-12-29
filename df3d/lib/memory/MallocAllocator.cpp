#include "MallocAllocator.h"

#include <df3d/lib/Utils.h>

namespace df3d {

// Taken from
// https://bitbucket.org/bitsquid/foundation

static_assert(sizeof(size_t) == sizeof(void*), "");

struct ChunkHeader
{
    size_t size;
};

// If we need to align the memory allocation we pad the header with this
// value after storing the size.
const size_t HEADER_PAD_VALUE = std::numeric_limits<size_t>::max();

// Given a pointer to the data, returns a pointer to the header before it.
inline ChunkHeader* GetHeader(void *data)
{
    auto tmp = (size_t *)data;
    while (tmp[-1] == HEADER_PAD_VALUE)
        --tmp;
    return (ChunkHeader *)tmp - 1;
}

// Aligns p to the specified alignment by moving it forward if necessary
// and returns the result.
inline void* AlignForward(void *p, size_t align)
{
    uintptr_t pi = uintptr_t(p);
    const size_t mod = pi % align;
    if (mod)
        pi += (align - mod);
    return (void *)pi;
}

MallocAllocator::MallocAllocator()
{

}

MallocAllocator::~MallocAllocator()
{
    DF3D_ASSERT(m_totalAllocated == 0);
    DF3D_ASSERT(m_totalAllocatedSize == 0);
}

void* MallocAllocator::alloc(size_t size, size_t alignment)
{
    std::lock_guard<std::mutex> lock(m_lock);

    auto sizeToAllocate = size + alignment + sizeof(ChunkHeader);

    auto header = (ChunkHeader*)malloc(sizeToAllocate);
    header->size = sizeToAllocate;

    // Align.
    void *dataPointer = AlignForward(header + 1, alignment);

    // Fill.
    size_t *tmp = (size_t*)(header + 1);
    while (tmp != dataPointer)
        *tmp++ = HEADER_PAD_VALUE;

    ++m_totalAllocated;
    m_totalAllocatedSize += sizeToAllocate;

    return dataPointer;
}

void MallocAllocator::dealloc(void *mem)
{
    if (!mem)
        return;

    std::lock_guard<std::mutex> lock(m_lock);

    auto header = GetHeader(mem);

    --m_totalAllocated;
    m_totalAllocatedSize -= header->size;

    DF3D_ASSERT(m_totalAllocated >= 0);

    free(header);
}

size_t MallocAllocator::bytesAllocated()
{
    return m_totalAllocatedSize;
}

}

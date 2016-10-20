#include "Handles.h"

namespace df3d {

static const HandleType HANDLE_INDEX_BITS = 19;
static const HandleType HANDLE_INDEX_MASK = (1 << HANDLE_INDEX_BITS) - 1;

static const HandleType HANDLE_GENERATION_BITS = 13;
static const HandleType HANDLE_GENERATION_MASK = (1 << HANDLE_GENERATION_BITS) - 1;

HandleBag::HandleBag(Allocator *allocator)
    : m_generations(allocator),
    m_freeList(allocator)
{

}

HandleBag::~HandleBag()
{

}

HandleType HandleBag::getNew()
{
    HandleType idx;
    if (m_freeList.empty())
    {
        m_generations.push_back(1);
        idx = m_generations.size() - 1;
    }
    else
    {
        idx = m_freeList.back();
        m_freeList.pop_back();
    }

    ++m_count;

    return MakeHandle(idx, m_generations[idx]);
}

void HandleBag::release(HandleType handle)
{
    DF3D_ASSERT(isValid(handle) && m_count > 0);

    auto idx = HandleIndex(handle);

    // Increment generation.
    ++m_generations[idx];
    // First generation should be 1.
    if (m_generations[idx] >= (1 << HANDLE_GENERATION_BITS))
        m_generations[idx] = 1;
    m_freeList.push_back(idx);

    --m_count;
}

bool HandleBag::isValid(HandleType handle) const
{
    auto idx = HandleIndex(handle);
    auto generation = HandleGeneration(handle);
    return (idx < m_generations.size()) && (m_generations[idx] == generation);
}

void HandleBag::reset()
{
    m_generations.clear();
    m_freeList.clear();
    m_generations.shrink_to_fit();
    m_freeList.shrink_to_fit();
    m_count = 0;
}

HandleType HandleBag::MakeHandle(HandleType index, HandleType generation)
{
    return index | (generation << HANDLE_INDEX_BITS);
}

HandleType HandleBag::HandleGeneration(HandleType handle)
{
    return (handle >> HANDLE_INDEX_BITS) & HANDLE_GENERATION_MASK;
}

HandleType HandleBag::HandleIndex(HandleType handle)
{
    return handle & HANDLE_INDEX_MASK;
}

}

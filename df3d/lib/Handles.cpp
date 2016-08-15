#include "Handles.h"

namespace df3d {

HandleBag::HandleBag(Allocator *allocator)
    : m_generations(allocator),
    m_freeList(allocator)
{

}

HandleBag::~HandleBag()
{

}

Handle HandleBag::getNew()
{
    uint32_t idx;
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

    return Handle(idx, m_generations[idx]);
}

void HandleBag::release(Handle handle)
{
    DF3D_ASSERT(isValid(handle) && m_count > 0);
    ++m_generations[handle.getIdx()];
    if (m_generations[handle.getIdx()] >= (1 << HANDLE_GENERATION_BITS))
        m_generations[handle.getIdx()] = 1;
    m_freeList.push_back(handle.getIdx());

    --m_count;
}

bool HandleBag::isValid(Handle handle) const
{
    return (handle.getIdx() < m_generations.size()) && (m_generations[handle.getIdx()] == handle.getGeneration());
}

void HandleBag::reset()
{
    m_generations.clear();
    m_freeList.clear();
    m_generations.shrink_to_fit();
    m_freeList.shrink_to_fit();
    m_count = 0;
}

}

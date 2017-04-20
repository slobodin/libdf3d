#pragma once

#include "containers/PodArray.h"
#include <cstdint>

namespace df3d {

using HandleType = uint32_t;

class HandleBag
{
    PodArray<HandleType> m_generations;
    PodArray<HandleType> m_freeList;
    uint32_t m_count = 0;

public:
    HandleBag(Allocator &allocator);
    ~HandleBag();

    HandleType getNew();
    void release(HandleType handle);

    bool isValid(HandleType handle) const;
    uint32_t getSize() const { return m_count; }
    bool empty() const { return getSize() == 0; }

    void reset();

    static HandleType MakeHandle(HandleType index, HandleType generation);
    static HandleType HandleGeneration(HandleType handle);
    static HandleType HandleIndex(HandleType handle);
};

#define DF3D_DECLARE_HANDLE(name) class name { \
    HandleType m_id; \
public: \
    name() : m_id(0) { } \
    explicit name(HandleType id) : m_id(id) { } \
    name(HandleType index, HandleType generation) : m_id(HandleBag::MakeHandle(index, generation)) { } \
    HandleType getID() const { return m_id; } \
    HandleType getIndex() const { return HandleBag::HandleIndex(m_id); } \
    HandleType getGeneration() const { return HandleBag::HandleGeneration(m_id); } \
    bool isValid() const { return m_id != 0; } \
    bool operator== (const name &other) const { return other.m_id == m_id; } \
    bool operator!= (const name &other) const { return other.m_id != m_id; } \
    bool operator< (const name &other) const { return m_id < other.m_id; } };

}

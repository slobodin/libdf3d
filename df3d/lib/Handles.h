#pragma once

#include "containers/PodArray.h"

namespace df3d {

static const uint32_t HANDLE_INDEX_BITS = 19;
static const uint32_t HANDLE_INDEX_MASK = (1 << HANDLE_INDEX_BITS) - 1;

static const uint32_t HANDLE_GENERATION_BITS = 13;
static const uint32_t HANDLE_GENERATION_MASK = (1 << HANDLE_GENERATION_BITS) - 1;

class Handle
{
    uint32_t m_id;

public:
    Handle() : m_id(0) { }
    explicit Handle(uint32_t id) : m_id(id) { }
    Handle(uint32_t idx, uint32_t generation) 
        : m_id(idx | (generation << HANDLE_INDEX_BITS))
    { }

    uint32_t getIdx() const { return m_id & HANDLE_INDEX_MASK; }
    uint32_t getGeneration() const { return (m_id >> HANDLE_INDEX_BITS) & HANDLE_GENERATION_MASK; }
    bool isValid() const { return m_id != 0; }
    uint32_t getID() const { return m_id; }

    bool operator== (const Handle &other) const { return m_id == other.m_id; }
    bool operator!= (const Handle &other) const { return m_id != other.m_id; }
    bool operator< (const Handle &other) const { return m_id < other.m_id; }
};

class DF3D_DLL HandleBag
{
    PodArray<uint32_t> m_generations;
    PodArray<uint32_t> m_freeList;
    uint32_t m_count = 0;

public:
    HandleBag(Allocator *allocator);
    ~HandleBag();

    Handle getNew();
    void release(Handle handle);

    bool isValid(Handle handle) const;
    uint32_t getSize() const { return m_count; }

    void reset();
};

#define DF3D_MAKE_HANDLE(name) struct name { \
    Handle handle; \
    uint32_t getID() const { return handle.getID(); } \
    uint32_t getIdx() const { return handle.getIdx(); } \
    bool isValid() const { return handle.isValid(); } \
    bool operator== (const name &other) const { return other.handle == handle; } \
    bool operator!= (const name &other) const { return other.handle != handle; } \
    bool operator< (const name &other) const { return handle < other.handle; } };

}

namespace std {

template <>
struct hash<df3d::Handle>
{
    std::size_t operator()(const df3d::Handle &handle) const
    {
        auto id = handle.getID();
        return std::hash<decltype(id)>()(id);
    }
};

}


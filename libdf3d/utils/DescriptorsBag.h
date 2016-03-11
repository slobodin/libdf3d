#pragma once

namespace df3d { namespace utils {

// TODO: not only int16_t

class DescriptorsBag
{
    int16_t m_maxSize;

    int16_t m_next = 0;

    std::list<int16_t> m_removed;
    std::list<int16_t> m_available;
    int16_t getNextId();

public:
    DescriptorsBag(int16_t maxSize = 0x7FFF);
    ~DescriptorsBag();

    int16_t getNew();
    void release(int16_t d);

    void cleanup();
};

} }

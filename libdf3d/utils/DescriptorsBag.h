#pragma once

namespace df3d { namespace utils {

// TODO: not only int16_t

class DescriptorsBag
{
    int16_t m_maxSize;

public:
    DescriptorsBag(int16_t maxSize = 0xFFFF);
    ~DescriptorsBag();

    int16_t getNew() const;
    void release(int16_t d);
};

} }

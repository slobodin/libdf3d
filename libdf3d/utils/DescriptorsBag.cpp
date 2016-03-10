#include "DescriptorsBag.h"

namespace df3d { namespace utils {

DescriptorsBag::DescriptorsBag(int16_t maxSize)
    : m_maxSize(maxSize)
{

}

DescriptorsBag::~DescriptorsBag()
{

}

int16_t DescriptorsBag::getNew() const
{
    static int16_t i;
    return i++;
}

void DescriptorsBag::release(int16_t d)
{
    // TODO_render
}

} }

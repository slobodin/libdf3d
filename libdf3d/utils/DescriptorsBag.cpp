#include "DescriptorsBag.h"

namespace df3d { namespace utils {

int16_t DescriptorsBag::getNextId()
{
    if (!m_available.empty())
    {
        auto res = m_available.front();
        m_available.pop_front();
        return res;
    }

    return m_next++;
}

DescriptorsBag::DescriptorsBag(int16_t maxSize)
    : m_maxSize(maxSize)
{

}

DescriptorsBag::~DescriptorsBag()
{

}

int16_t DescriptorsBag::getNew()
{
    if (m_next >= m_maxSize)
        return {};

    return getNextId();
}

void DescriptorsBag::release(int16_t d)
{
    m_removed.push_back(d);
}

void DescriptorsBag::cleanup()
{
    m_available.insert(m_available.end(), m_removed.begin(), m_removed.end());
    m_removed.clear();
}

} }

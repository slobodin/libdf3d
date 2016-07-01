#include "MemoryDataSource.h"

namespace df3d {

MemoryDataSource::MemoryDataSource(const uint8_t *buffer, int32_t size, const std::string &fileName)
    : m_buffer(std::move(buffer)),
    m_size(size),
    m_fileName(fileName)
{
    m_current = m_buffer;
}

MemoryDataSource::~MemoryDataSource()
{

}

size_t MemoryDataSource::read(void *buffer, size_t sizeInBytes)
{
    if (m_current + sizeInBytes > m_buffer + m_size)
        sizeInBytes = m_buffer + m_size - m_current;

    memcpy(buffer, m_current, sizeInBytes);

    m_current += sizeInBytes;

    return sizeInBytes;
}

size_t MemoryDataSource::getSize()
{
    return m_size;
}

int32_t MemoryDataSource::tell()
{
    return m_current - m_buffer;
}

bool MemoryDataSource::seek(int32_t offset, SeekDir origin)
{
    if (origin == SeekDir::CURRENT)
        m_current += offset;
    else if (origin == SeekDir::BEGIN)
        m_current = m_buffer + offset;
    else if (origin == SeekDir::END)
        m_current = m_buffer + m_size + offset;
    else
        return false;

    return (m_current - m_buffer) <= m_size;
}

}

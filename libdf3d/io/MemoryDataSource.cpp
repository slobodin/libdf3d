#include "MemoryDataSource.h"

namespace df3d {

MemoryDataSource::MemoryDataSource(unique_ptr<unsigned char[]> &&buffer, int32_t size, const std::string &fileName)
    : FileDataSource(fileName),
    m_buffer(std::move(buffer)),
    m_size(size)
{
    m_current = m_buffer.get();
}

MemoryDataSource::~MemoryDataSource()
{

}

bool MemoryDataSource::valid() const
{
    return m_buffer != nullptr;
}

size_t MemoryDataSource::getRaw(void *buffer, size_t sizeInBytes)
{
    if (m_current + sizeInBytes > m_buffer.get() + m_size)
        sizeInBytes = m_buffer.get() + m_size - m_current;

    memcpy(buffer, m_current, sizeInBytes);

    m_current += sizeInBytes;

    return sizeInBytes;
}

size_t MemoryDataSource::getSizeInBytes()
{
    return m_size;
}

size_t MemoryDataSource::tell()
{
    return m_current - m_buffer.get();
}

bool MemoryDataSource::seek(size_t offset, std::ios_base::seekdir origin)
{
    if (origin == std::ios_base::cur)
        m_current += offset;
    else if (origin == std::ios_base::beg)
        m_current = m_buffer.get() + offset;
    else if (origin == std::ios_base::end)
        m_current = m_buffer.get() + m_size - offset;
    else
        return false;

    return (m_current - m_buffer.get()) <= m_size;
}

}

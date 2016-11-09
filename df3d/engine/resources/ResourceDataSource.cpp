#include "ResourceDataSource.h"

#include <df3d/lib/os/PlatformFile.h>

namespace df3d {

class FileDataSource : public ResourceDataSource
{
    unique_ptr<PlatformFile> m_file;

public:
    FileDataSource(unique_ptr<PlatformFile> file)
        : m_file(std::move(file))
    {
    }

    ~FileDataSource()
    {

    }

    size_t read(void *buffer, size_t sizeInBytes) override
    {
        return m_file->read(buffer, sizeInBytes);
    }

    size_t getSize() override
    {
        return m_file->getSize();
    }

    int32_t tell() override
    {
        return m_file->tell();
    }

    bool seek(int32_t offset, SeekDir origin) override
    {
        return m_file->seek(offset, origin);
    }
};

class MemoryDataSource : public ResourceDataSource
{
    const uint8_t *m_buffer;
    const uint8_t *m_current;
    int32_t m_size;

public:
    MemoryDataSource(const uint8_t *buffer, int32_t size)
        : m_buffer(std::move(buffer)),
        m_size(size)
    {
        m_current = m_buffer;
    }

    ~MemoryDataSource()
    {

    }

    size_t read(void *buffer, size_t sizeInBytes) override
    {
        if (m_current + sizeInBytes > m_buffer + m_size)
            sizeInBytes = m_buffer + m_size - m_current;

        memcpy(buffer, m_current, sizeInBytes);

        m_current += sizeInBytes;

        return sizeInBytes;
    }

    size_t getSize() override
    {
        return m_size;
    }

    int32_t tell() override
    {
        return m_current - m_buffer;
    }

    bool seek(int32_t offset, SeekDir origin) override
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
};

ResourceDataSource* CreateFileDataSource(const char *path, Allocator &allocator)
{
    auto platformFile = PlatformOpenFile(path);
    if (!platformFile)
        return nullptr;

    return MAKE_NEW(allocator, FileDataSource)(std::move(platformFile));
}

ResourceDataSource* CreateMemoryDataSource(const uint8_t *buffer, int32_t size, Allocator &allocator)
{
    return MAKE_NEW(allocator, MemoryDataSource)(buffer, size);
}

}

#include "FileDataSourceDesktop.h"

namespace df3d { namespace platform_impl {

FileDataSourceDesktop::FileDataSourceDesktop(const std::string &fileName)
    : FileDataSource(fileName),
    m_file(nullptr)
{
    m_file = fopen(fileName.c_str(), "rb");
    if (m_file)
    {
        if (seek(0, std::ios_base::end))
        {
            m_sizeInBytes = tell();
            seek(0, std::ios_base::beg);
        }
    }
    else
    {
        DFLOG_WARN("Can not open file %s", fileName.c_str());
    }
}

FileDataSourceDesktop::~FileDataSourceDesktop()
{
    if (m_file)
        fclose(m_file);
}

bool FileDataSourceDesktop::valid() const
{
    return m_file != nullptr;
}

size_t FileDataSourceDesktop::getRaw(void *buffer, size_t sizeInBytes)
{
    if (!m_file)
        return 0;
    return fread(buffer, 1, sizeInBytes, m_file);
}

size_t FileDataSourceDesktop::getSizeInBytes()
{
    return m_sizeInBytes;
}

size_t FileDataSourceDesktop::tell()
{
    if (!m_file)
        return -1;
    return ftell(m_file);
}

bool FileDataSourceDesktop::seek(int32_t offset, std::ios_base::seekdir origin)
{
    int whence;
    if (origin == std::ios_base::cur)
        whence = SEEK_CUR;
    else if (origin == std::ios_base::beg)
        whence = SEEK_SET;
    else if (origin == std::ios_base::end)
        whence = SEEK_END;
    else
        return false;

    return fseek(m_file, offset, whence) == 0;
}

} }

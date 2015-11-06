#include "FileDataSourceDesktop.h"

namespace df3d { namespace platform {

FileDataSourceDesktop::FileDataSourceDesktop(const std::string &fileName)
    : resources::FileDataSource(fileName),
    m_file(nullptr)
{
    m_file = fopen(fileName.c_str(), "rb");
    if (!m_file)
        glog << "Can not open file" << fileName << base::logwarn;
}

FileDataSourceDesktop::~FileDataSourceDesktop()
{
    close();
}

bool FileDataSourceDesktop::valid() const
{
    return m_file != nullptr;
}

void FileDataSourceDesktop::close()
{
    if (m_file)
        fclose(m_file);
    m_file = nullptr;
}

size_t FileDataSourceDesktop::getRaw(void *buffer, size_t sizeInBytes)
{
    if (!m_file)
        return 0;
    return fread(buffer, 1, sizeInBytes, m_file);
}

int64_t FileDataSourceDesktop::getSize()
{
    if (!m_file)
        return 0;

    int64_t result = 0;
    if (seek(0, std::ios_base::end))
    {
        result = tell();
        seek(0, std::ios_base::beg);
    }

    return result;
}

int64_t FileDataSourceDesktop::tell()
{
    if (!m_file)
        return -1;
    return ftell(m_file);
}

bool FileDataSourceDesktop::seek(int64_t offset, std::ios_base::seekdir origin)
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

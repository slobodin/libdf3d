#include "df3d_pch.h"
#include "FileDataSource.h"

namespace df3d { namespace resources {

FileDataSource::FileDataSource(const char *fileName)
    : m_file(nullptr),
    m_filePath(fileName)
{
    m_file = fopen(fileName, "rb");
    if (!m_file)
        base::glog << "Can not open file" << fileName << base::logcritical;
}

FileDataSource::~FileDataSource()
{
    close();
}

bool FileDataSource::valid() const
{
    return m_file != nullptr;
}

void FileDataSource::close()
{
    if (m_file)
        fclose(m_file);
    m_file = nullptr;
}

size_t FileDataSource::getRaw(void *buffer, size_t sizeInBytes)
{
    if (!m_file)
        return 0;
    return fread(buffer, 1, sizeInBytes, m_file);
}

long FileDataSource::getSize()
{
    if (!m_file)
        return 0;

    long result = 0;
    if (seek(0, std::ios_base::end))
    {
        result = tell();
        seek(0, std::ios_base::beg);
    }

    return result;
}

long FileDataSource::tell()
{
    if (!m_file)
        return -1;
    return ftell(m_file);
}

bool FileDataSource::seek(long offset, std::ios_base::seekdir origin)
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

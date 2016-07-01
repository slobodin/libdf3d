#include "FileDataSource.h"

#include <df3d/lib/os/PlatformFile.h>

namespace df3d {

FileDataSource::FileDataSource(const std::string &fileName, unique_ptr<PlatformFile> &&file)
    : m_filePath(fileName),
    m_file(std::move(file))
{

}

size_t FileDataSource::read(void *buffer, size_t sizeInBytes)
{
    return m_file->read(buffer, sizeInBytes);
}

size_t FileDataSource::getSize()
{
    return m_file->getSize();
}

int32_t FileDataSource::tell()
{
    return m_file->tell();
}

bool FileDataSource::seek(int32_t offset, SeekDir origin)
{
    return m_file->seek(offset, origin);
}

}

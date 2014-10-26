#include "df3d_pch.h"
#include "FileDataSource.h"

// awful! But I experienced crashes when two threads simultaneously call SDL_RWclose
std::mutex g_lock;

namespace df3d { namespace resources {

FileDataSource::FileDataSource(const char *fileName)
    : m_file(nullptr),
    m_filePath(fileName)
{
    std::lock_guard<std::mutex> lock(g_lock);

    m_file = SDL_RWFromFile(fileName, "rb");
    if (!m_file)
    {
        base::glog << "Can not open file" << fileName << base::logcritical;
        return;
    }
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
    std::lock_guard<std::mutex> lock(g_lock);
    if (m_file)
        SDL_RWclose(m_file);
    m_file = nullptr;
}

size_t FileDataSource::getRaw(void *buffer, size_t sizeInBytes)
{
    return SDL_RWread(m_file, buffer, 1, sizeInBytes);
}

size_t FileDataSource::getSize() const
{
    return (size_t)SDL_RWsize(m_file);
}

size_t FileDataSource::tell()
{
    return (size_t)SDL_RWtell(m_file);
}

bool FileDataSource::seek(long offset, std::ios_base::seekdir origin)
{
    int whence;
    if (origin == std::ios_base::cur)
        whence = RW_SEEK_CUR;
    else if (origin == std::ios_base::beg)
        whence = RW_SEEK_SET;
    else if (origin == std::ios_base::end)
        whence = RW_SEEK_END;
    else
        return false;

    return SDL_RWseek(m_file, offset, whence) != -1;
}

} }

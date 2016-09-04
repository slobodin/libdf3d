#include "FileSystem.h"

namespace df3d {

FileSystem::FileSystem()
{

}

FileSystem::~FileSystem()
{

}

shared_ptr<DataSource> FileSystem::open(const char *path)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    if (!m_fileDevice)
        return nullptr;

    return m_fileDevice->openDataSource(path);
}

}

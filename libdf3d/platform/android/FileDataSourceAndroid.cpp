#include "FileDataSourceAndroid.h"

namespace df3d { namespace platform_impl {

AAssetManager *FileDataSourceAndroid::m_assetMgr = nullptr;

FileDataSourceAndroid::FileDataSourceAndroid(const char *fileName)
    : FileDataSource(fileName),
    m_file(nullptr),
    m_current(0)
{
    m_file = AAssetManager_open(m_assetMgr, fileName, AASSET_MODE_UNKNOWN);
    if (!m_file)
        glog << "Can not open file" << fileName << logwarn;
}

FileDataSourceAndroid::~FileDataSourceAndroid()
{
    close();
}

bool FileDataSourceAndroid::valid() const
{
    return m_file != nullptr;
}

void FileDataSourceAndroid::close()
{
    if (m_file)
        AAsset_close(m_file);
    m_file = nullptr;
    m_current = 0;
}

size_t FileDataSourceAndroid::getRaw(void *buffer, size_t sizeInBytes)
{
    if (!m_file)
        return 0;

    auto bytesRead = AAsset_read(m_file, buffer, sizeInBytes);
    m_current += bytesRead;

    return bytesRead;
}

int64_t FileDataSourceAndroid::getSize()
{
    if (!m_file)
        return 0;

    return AAsset_getLength(m_file);
}

int64_t FileDataSourceAndroid::tell()
{
    if (!m_file)
        return -1;
    return m_current;
}

bool FileDataSourceAndroid::seek(int64_t offset, std::ios_base::seekdir origin)
{
    int whence;
    if (origin == std::ios_base::cur)
    {
        whence = SEEK_CUR;
        m_current += offset;
    }
    else if (origin == std::ios_base::beg)
    {
        whence = SEEK_SET;
        m_current = offset;
    }
    else if (origin == std::ios_base::end)
    {
        whence = SEEK_END;
        m_current = getSize() + offset;
    }
    else
        return false;

    return AAsset_seek(m_file, offset, whence) == 0;
}

void FileDataSourceAndroid::setAssetManager(AAssetManager *mgr)
{
    if (!mgr)
        glog << "Setting up NULL AAssetManager" << logwarn;

    m_assetMgr = mgr;

    glog << "AAssetManager was set up" << logmess;
}

} }

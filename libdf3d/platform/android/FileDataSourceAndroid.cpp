#include "FileDataSourceAndroid.h"

namespace df3d {

namespace platform_impl {

AAssetManager *FileDataSourceAndroid::m_assetMgr = nullptr;

FileDataSourceAndroid::FileDataSourceAndroid(const std::string &fileName)
    : FileDataSource(fileName),
    m_file(nullptr)
{
    m_file = AAssetManager_open(m_assetMgr, fileName.c_str(), AASSET_MODE_UNKNOWN);
    if (!m_file)
        DFLOG_WARN("Can not open file %s", fileName.c_str());
}

FileDataSourceAndroid::~FileDataSourceAndroid()
{
    if (m_file)
        AAsset_close(m_file);
}

bool FileDataSourceAndroid::valid() const
{
    return m_file != nullptr;
}

size_t FileDataSourceAndroid::getRaw(void *buffer, size_t sizeInBytes)
{
    if (!m_file)
        return 0;

    return AAsset_read(m_file, buffer, sizeInBytes);
}

size_t FileDataSourceAndroid::getSizeInBytes()
{
    if (!m_file)
        return 0;

    return AAsset_getLength(m_file);
}

int32_t FileDataSourceAndroid::tell()
{
    return AAsset_getLength(m_file) - AAsset_getRemainingLength(m_file);
}

bool FileDataSourceAndroid::seek(int32_t offset, std::ios_base::seekdir origin)
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

    return AAsset_seek(m_file, offset, whence) != -1;
}

void FileDataSourceAndroid::setAssetManager(AAssetManager *mgr)
{
    if (!mgr)
        DFLOG_WARN("Setting up NULL AAssetManager");

    m_assetMgr = mgr;

    DFLOG_MESS("AAssetManager was set up");
}

} }

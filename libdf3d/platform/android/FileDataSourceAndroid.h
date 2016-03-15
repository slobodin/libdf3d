#pragma once

#include <libdf3d/io/FileDataSource.h>
#include <android/asset_manager.h>

namespace df3d { namespace platform_impl {

class DF3D_DLL FileDataSourceAndroid : public FileDataSource
{
    AAsset *m_file = nullptr;
    static AAssetManager *m_assetMgr;
    size_t m_current = 0;

public:
    FileDataSourceAndroid(const char *fileName);
    ~FileDataSourceAndroid();

    bool valid() const override;

    size_t getRaw(void *buffer, size_t sizeInBytes) override;
    size_t getSizeInBytes() override;

    size_t tell() override;
    bool seek(size_t offset, std::ios_base::seekdir origin) override;

    static void setAssetManager(AAssetManager *mgr);

    static AAssetManager* getAAssetManager() { return m_assetMgr; }
};

} }

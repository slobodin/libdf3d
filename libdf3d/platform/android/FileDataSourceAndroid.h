#pragma once

#include <libdf3d/io/FileDataSource.h>
#include <android/asset_manager.h>

namespace df3d { namespace platform_impl {

class DF3D_DLL FileDataSourceAndroid : public FileDataSource
{
    AAsset *m_file = nullptr;
    static AAssetManager *m_assetMgr;
    int64_t m_current = 0;

public:
    FileDataSourceAndroid(const char *fileName);
    ~FileDataSourceAndroid();

    virtual bool valid() const override;
    virtual void close() override;

    virtual size_t getRaw(void *buffer, size_t sizeInBytes) override;
    virtual int64_t getSize() override;

    virtual int64_t tell() override;
    virtual bool seek(int64_t offset, std::ios_base::seekdir origin) override;

    static void setAssetManager(AAssetManager *mgr);

    static AAssetManager* getAAssetManager() { return m_assetMgr; }
};

} }

#pragma once

#include <resources/FileDataSource.h>

namespace df3d { namespace platform {

class DF3D_DLL FileDataSourceDesktop : public resources::FileDataSource
{
    FILE *m_file = nullptr;

public:
    FileDataSourceDesktop(const char *fileName);
    ~FileDataSourceDesktop();

    virtual bool valid() const override;
    virtual void close() override;

    virtual size_t getRaw(void *buffer, size_t sizeInBytes) override;
    virtual int64_t getSize() override;

    virtual int64_t tell() override;
    virtual bool seek(int64_t offset, std::ios_base::seekdir origin) override;
};

} }

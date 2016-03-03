#pragma once

#include <libdf3d/io/FileDataSource.h>

namespace df3d { namespace platform_impl {

class DF3D_DLL FileDataSourceDesktop : public FileDataSource
{
    FILE *m_file = nullptr;

public:
    FileDataSourceDesktop(const std::string &fileName);
    ~FileDataSourceDesktop();

    bool valid() const override;

    size_t getRaw(void *buffer, size_t sizeInBytes) override;
    int64_t getSizeInBytes() override;

    int64_t tell() override;
    bool seek(int64_t offset, std::ios_base::seekdir origin) override;
};

} }

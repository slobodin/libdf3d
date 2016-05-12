#pragma once

#include <libdf3d/io/FileDataSource.h>

namespace df3d { namespace platform_impl {

class DF3D_DLL FileDataSourceDesktop : public FileDataSource
{
    FILE *m_file = nullptr;
    size_t m_sizeInBytes = 0;

public:
    FileDataSourceDesktop(const std::string &fileName);
    ~FileDataSourceDesktop();

    bool valid() const override;

    size_t getRaw(void *buffer, size_t sizeInBytes) override;
    size_t getSizeInBytes() override;

    size_t tell() override;
    bool seek(int32_t offset, std::ios_base::seekdir origin) override;
};

} }

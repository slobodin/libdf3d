#pragma once

#include "FileDataSource.h"

namespace df3d {

class DF3D_DLL MemoryDataSource : public FileDataSource
{
    const uint8_t *m_buffer;
    const uint8_t *m_current;
    int32_t m_size;

public:
    MemoryDataSource(const uint8_t *buffer, int32_t size, const std::string &fileName);
    ~MemoryDataSource();

    bool valid() const override;

    size_t getRaw(void *buffer, size_t sizeInBytes) override;
    size_t getSizeInBytes() override;

    int32_t tell() override;
    bool seek(int32_t offset, std::ios_base::seekdir origin) override;
};

}

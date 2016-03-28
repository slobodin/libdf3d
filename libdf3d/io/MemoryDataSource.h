#pragma once

#include "FileDataSource.h"

namespace df3d {

class MemoryDataSource : public FileDataSource
{
    unique_ptr<uint8_t[]> m_buffer;
    uint8_t *m_current;
    int32_t m_size;

public:
    MemoryDataSource(unique_ptr<uint8_t[]> &&buffer, int32_t size, const std::string &fileName);
    ~MemoryDataSource();

    bool valid() const override;

    size_t getRaw(void *buffer, size_t sizeInBytes) override;
    size_t getSizeInBytes() override;

    size_t tell() override;
    bool seek(size_t offset, std::ios_base::seekdir origin) override;
};

}

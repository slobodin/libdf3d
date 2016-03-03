#pragma once

#include "FileDataSource.h"

namespace df3d {

class MemoryDataSource : public FileDataSource
{
    unique_ptr<unsigned char[]> m_buffer;
    unsigned char *m_current;
    int64_t m_size;

public:
    MemoryDataSource(unique_ptr<unsigned char[]> &&buffer, int64_t size, const std::string &fileName);
    ~MemoryDataSource();

    bool valid() const override;

    size_t getRaw(void *buffer, size_t sizeInBytes) override;
    int64_t getSizeInBytes() override;

    int64_t tell() override;
    bool seek(int64_t offset, std::ios_base::seekdir origin) override;
};

}

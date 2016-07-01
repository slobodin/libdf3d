#pragma once

#include "DataSource.h"

namespace df3d {

class DF3D_DLL MemoryDataSource : public DataSource
{
    const uint8_t *m_buffer;
    const uint8_t *m_current;
    int32_t m_size;
    std::string m_fileName;

public:
    MemoryDataSource(const uint8_t *buffer, int32_t size, const std::string &fileName);
    ~MemoryDataSource();

    size_t read(void *buffer, size_t sizeInBytes) override;
    size_t getSize() override;

    int32_t tell() override;
    bool seek(int32_t offset, SeekDir origin) override;

    const std::string& getPath() const { return m_fileName; }
};

}

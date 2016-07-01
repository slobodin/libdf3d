#pragma once

#include "DataSource.h"

namespace df3d {

class PlatformFile;

class DF3D_DLL FileDataSource : public DataSource
{
    std::string m_filePath;
    unique_ptr<PlatformFile> m_file;

public:
    FileDataSource(const std::string &fileName, unique_ptr<PlatformFile> &&file);
    ~FileDataSource() = default;

    size_t read(void *buffer, size_t sizeInBytes) override;
    size_t getSize() override;

    int32_t tell() override;
    bool seek(int32_t offset, SeekDir origin) override;

    const std::string& getPath() const override { return m_filePath; }
};

}


#pragma once

namespace df3d {

// TODO:
// Implements DataSource.

class DF3D_DLL FileDataSource : utils::NonCopyable
{
    std::string m_filePath;

public:
    FileDataSource(const std::string &fileName)
        : m_filePath(fileName)
    {

    }

    virtual ~FileDataSource() { }

    virtual bool valid() const = 0;

    virtual size_t getRaw(void *buffer, size_t sizeInBytes) = 0;
    virtual size_t getSizeInBytes() = 0;

    virtual size_t tell() = 0;
    virtual bool seek(int32_t offset, std::ios_base::seekdir origin) = 0;

    template<typename T>
    bool getAsObjects(T *output, size_t numElements)
    {
        size_t bytes = numElements * sizeof(T);
        auto got = getRaw(output, bytes);

        return got == bytes;
    }

    const std::string& getPath() const { return m_filePath; }
};

}


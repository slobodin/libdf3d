#pragma once

namespace df3d {

class DF3D_DLL DataSource
{
public:
    DataSource() = default;
    virtual ~DataSource() = default;

    virtual size_t read(void *buffer, size_t sizeInBytes) = 0;
    virtual size_t getSize() = 0;

    virtual int32_t tell() = 0;
    virtual bool seek(int32_t offset, SeekDir origin) = 0;

    virtual const std::string& getPath() const = 0;
};

template<typename T>
bool DataSourceGetObjects(DataSource *source, T *output, size_t numElements)
{
    if (!source)
        return false;

    size_t bytes = numElements * sizeof(T);
    auto got = source->read(output, bytes);

    return got == bytes;
}

}

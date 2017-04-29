#pragma once

namespace df3d {

class ResourceDataSource
{
public:
    ResourceDataSource() = default;
    virtual ~ResourceDataSource() = default;

    virtual size_t read(void *buffer, size_t sizeInBytes) = 0;
    virtual size_t getSize() = 0;

    virtual int32_t tell() = 0;
    virtual bool seek(int32_t offset, SeekDir origin) = 0;

    template<typename T>
    bool getObjects(T *output, size_t numElements)
    {
        size_t bytes = numElements * sizeof(T);
        auto got = read(output, bytes);

        return got == bytes;
    }
};

ResourceDataSource* CreateFileDataSource(const char *path, Allocator &allocator);
ResourceDataSource* CreateMemoryDataSource(const uint8_t *buffer, int32_t size, Allocator &allocator);

}

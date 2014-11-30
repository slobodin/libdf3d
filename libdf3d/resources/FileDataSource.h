#pragma once

namespace df3d { namespace resources {

class DF3D_DLL FileDataSource : boost::noncopyable
{
    FILE *m_file = nullptr;
    std::string m_filePath;

public:
    FileDataSource(const char *fileName);
    ~FileDataSource();

    bool valid() const;
    void close();

    template<typename T>
    bool getAsObjects(T *output, size_t numElements);

    size_t getRaw(void *buffer, size_t sizeInBytes);
    //! Returns size in bytes.
    long getSize();

    long tell();
    bool seek(long offset, std::ios_base::seekdir origin);

    const std::string &getPath() const { return m_filePath; }
};

template<typename T>
bool FileDataSource::getAsObjects(T *output, size_t numElements)
{
    size_t bytes = numElements * sizeof(T);
    auto got = getRaw(output, bytes);

    return got == bytes;
}

} }
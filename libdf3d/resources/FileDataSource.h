#pragma once

struct SDL_RWops;

namespace df3d { namespace resources {

class DF3D_DLL FileDataSource : boost::noncopyable
{
    SDL_RWops *m_file;

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
    size_t getSize() const;

    size_t tell();
    bool seek(long offset, std::ios_base::seekdir origin);

    const std::string &getPath() const { return m_filePath; }

    SDL_RWops *getSdlRwops() const { return m_file; }
};

template<typename T>
bool FileDataSource::getAsObjects(T *output, size_t numElements)
{
    size_t bytes = numElements * sizeof(T);
    auto got = getRaw(output, bytes);

    return got == bytes;
}

} }
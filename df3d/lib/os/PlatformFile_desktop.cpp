#include "PlatformFile.h"

#if defined(DF3D_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined(DF3D_LINUX) || defined(DF3D_MACOSX)
#include <unistd.h>
#else
#error "Please implement"
#endif

namespace df3d {

class PlatformFileDesktop : public PlatformFile
{
    FILE *m_file;
    size_t m_sizeInBytes;

public:
    PlatformFileDesktop(FILE *file)
        : m_file(file),
        m_sizeInBytes(0)
    {
        if (seek(0, SeekDir::END))
        {
            m_sizeInBytes = tell();
            seek(0, SeekDir::BEGIN);
        }
    }

    ~PlatformFileDesktop()
    {
        fclose(m_file);
    }

    size_t getSize() override
    {
        return m_sizeInBytes;
    }

    int32_t tell() override
    {
        return ftell(m_file);
    }

    bool seek(int32_t offset, SeekDir origin) override
    {
        int whence;
        if (origin == SeekDir::CURRENT)
            whence = SEEK_CUR;
        else if (origin == SeekDir::BEGIN)
            whence = SEEK_SET;
        else if (origin == SeekDir::END)
            whence = SEEK_END;
        else
            return false;

        return fseek(m_file, offset, whence) == 0;
    }

    size_t read(void *buffer, size_t sizeInBytes) override
    {
        return fread(buffer, 1, sizeInBytes, m_file);
    }
};

unique_ptr<PlatformFile> PlatformOpenFile(const char *path)
{
    auto fh = fopen(path, "rb");
    if (fh)
        return make_unique<PlatformFileDesktop>(fh);

    DFLOG_WARN("Can not open file %s", path);
    return nullptr;
}

bool PlatformFileExists(const char *path)
{
#if defined(DF3D_WINDOWS)
    DWORD attrs = GetFileAttributes(path);
    return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
#elif defined(DF3D_LINUX) || defined(DF3D_MACOSX)
    return access(path, F_OK) != -1;
#else
#error "Please implement"
#endif
}

}

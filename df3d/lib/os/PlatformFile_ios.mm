#include <df3d/df3d.h>
#include "PlatformFile.h"
#import <Foundation/Foundation.h>

namespace df3d {

class PlatformFileIOS : public PlatformFile
{
    FILE *m_file;
    size_t m_sizeInBytes;

public:
    PlatformFileIOS(FILE *file)
        : m_file(file),
        m_sizeInBytes(0)
    {
        if (seek(0, SeekDir::END))
        {
            m_sizeInBytes = tell();
            seek(0, SeekDir::BEGIN);
        }
    }

    PlatformFileIOS()
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


static NSString* GetBundlePath(const char *path)
{
    NSString *pathNs = [NSString stringWithUTF8String:path];
    return [[NSBundle mainBundle] pathForResource:pathNs ofType:nil];
}

unique_ptr<PlatformFile> PlatformOpenFile(const char *path)
{
    NSString *bundlePath = GetBundlePath(path);

    auto fh = fopen([bundlePath UTF8String], "rb");
    if (fh)
        return make_unique<PlatformFileIOS>(fh);

    DFLOG_WARN("Can not open file %s", [bundlePath UTF8String]);
    return nullptr;
}

bool PlatformFileExists(const char *path)
{
    return [[NSFileManager defaultManager] fileExistsAtPath:GetBundlePath(path)];
}

}

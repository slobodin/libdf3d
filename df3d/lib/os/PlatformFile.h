#pragma once

#include <df3d/lib/NonCopyable.h>
#include <cstdint>
#include <df3d/Common.h>
#include <cstdint>

namespace df3d {

class PlatformFile : NonCopyable
{
public:
    PlatformFile() = default;
    virtual ~PlatformFile() = default;

    virtual size_t getSize() = 0;

    virtual int32_t tell() = 0;
    virtual bool seek(int32_t offset, SeekDir origin) = 0;
    virtual size_t read(void *buffer, size_t sizeInBytes) = 0;
};

// TODO: read only FS for now.
// TODO: rename because it's used for reading data from game package (apk, game data, etc).
unique_ptr<PlatformFile> PlatformOpenFile(const char *path);
bool PlatformFileExists(const char *path);

}

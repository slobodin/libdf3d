#include "df3d_pch.h"
#include "../Storage.h"

#include <utils/JsonHelpers.h>

namespace df3d { namespace platform {

class LinuxStorage : public Storage
{
public:
    LinuxStorage(const char *filename)
        : Storage(filename)
    {

    }

    virtual void save() override
    {

    }
};

Storage *Storage::create(const char *filename)
{
    return new LinuxStorage(filename);
}

} }

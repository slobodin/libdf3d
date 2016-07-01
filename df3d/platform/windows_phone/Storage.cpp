#include "df3d_pch.h"
#include "../Storage.h"

#include <utils/JsonHelpers.h>

namespace df3d { namespace platform {

class WindowsRTStorage : public Storage
{
public:
    WindowsRTStorage(const char *filename)
        : Storage(filename)
    {

    }

    virtual void save() override
    {

    }
};

Storage *Storage::create(const char *filename)
{
    return new WindowsRTStorage(filename);
}

} }

#include "df3d_pch.h"
#include "../Storage.h"

namespace df3d { namespace platform {

class AndroidStorage : public Storage
{
public:
    AndroidStorage(const char *filename)
        : Storage(filename)
    {

    }

    virtual void save() override
    {

    }
};

Storage *Storage::create(const char *filename)
{
    return new AndroidStorage(filename);
}

} }

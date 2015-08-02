#include "df3d_pch.h"
#include "../Storage.h"

namespace df3d { namespace platform {

class MacosxStorage : public Storage
{
public:
    MacosxStorage(const std::string &filename)
        : Storage(filename)
    {

    }

    virtual void save() override
    {

    }
};

Storage *Storage::create(const std::string &filename)
{
    return new MacosxStorage(filename);
}

} }

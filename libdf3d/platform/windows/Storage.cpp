#include "df3d_pch.h"
#include "../Storage.h"

#include <utils/JsonHelpers.h>

namespace df3d { namespace platform { 

class WindowsStorage : public Storage
{
public:
    WindowsStorage(const std::string &filename)
        : Storage(filename)
    {
        auto root = utils::jsonLoadFromFile(filename);
        if (!!root)
        {
            df3d::base::glog << "Failed to init Storage. Can not open file" << df3d::base::logwarn;
            return;
        }
    }

    virtual void save() override
    {

    }
};

Storage *Storage::create(const std::string &filename)
{
    return new WindowsStorage(filename);
}

} }

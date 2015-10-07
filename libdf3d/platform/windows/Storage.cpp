#include "../Storage.h"

#include <utils/JsonUtils.h>

namespace df3d { namespace platform { 

class WindowsStorage : public Storage
{
public:
    WindowsStorage(const std::string &filename)
        : Storage(filename)
    {
        auto root = utils::json::fromFile(filename);
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

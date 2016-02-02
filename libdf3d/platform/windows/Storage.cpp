#include <libdf3d/io/Storage.h>

#include <libdf3d/utils/JsonUtils.h>

namespace df3d { namespace platform_impl {

class WindowsStorage : public Storage
{
public:
    WindowsStorage(const std::string &filename)
        : Storage(filename)
    {
        auto root = utils::json::fromFile(filename);
        if (!!root)
        {
            df3d::glog << "Failed to init Storage. Can not open file" << df3d::logwarn;
            return;
        }
    }

    virtual void save() override
    {

    }
};

}

Storage *Storage::create(const std::string &filename)
{
    return new platform_impl::WindowsStorage(filename);
}

}

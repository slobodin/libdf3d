#include <libdf3d/io/Storage.h>

#include <libdf3d/utils/JsonUtils.h>

namespace df3d { namespace platform_impl {

class WindowsStorage : public Storage
{
    void saveToFileSystem(const std::string &data) override
    {

    }

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
};

}

Storage* Storage::create(const std::string &filename)
{
    return new platform_impl::WindowsStorage(filename);
}

}

#include <libdf3d/io/Storage.h>

#include <libdf3d/utils/JsonUtils.h>

namespace df3d { namespace platform_impl {

class AndroidStorage : public Storage
{
    void saveToFileSystem(const std::string &data) override
    {
        df3d::glog << "AndroidStorage::saveToFileSystem is not implemented" << df3d::logwarn;
    }

    bool getFromFileSystem(std::string &outStr) override
    {
        df3d::glog << "AndroidStorage::getFromFileSystem is not implemented" << df3d::logwarn;

        return false;
    }

public:
    AndroidStorage(const std::string &filename)
        : Storage(filename)
    {

    }
};

}

Storage* Storage::create(const std::string &filename)
{
    return new platform_impl::AndroidStorage(filename);
}

}

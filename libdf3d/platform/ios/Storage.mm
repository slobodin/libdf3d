#include <libdf3d/df3d.h>
#include <libdf3d/io/Storage.h>

#include <libdf3d/utils/JsonUtils.h>

namespace df3d { namespace platform_impl {

class IOSStorage : public Storage
{
    void saveToFileSystem(const uint8_t *data, size_t size) override
    {

    }

    bool getFromFileSystem(uint8_t **data, size_t *size) override
    {
        return false;
    }

public:
    IOSStorage(const std::string &filename)
        : Storage(filename)
    {

    }
};

}

Storage* Storage::create(const std::string &filename)
{
    return new platform_impl::IOSStorage(filename);
}

}

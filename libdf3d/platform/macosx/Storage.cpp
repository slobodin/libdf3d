#include <libdf3d/io/Storage.h>

namespace df3d { namespace platform_impl {

class MacosxStorage : public Storage
{
    void saveToFileSystem(const uint8_t *data, size_t size) override
    {

    }

    bool getFromFileSystem(uint8_t **data, size_t *size) override
    {
        return false;
    }

public:
    MacosxStorage(const std::string &filename)
        : Storage(filename)
    {

    }
};

}

Storage *Storage::create(const std::string &filename)
{
    return new platform_impl::MacosxStorage(filename);
}

}

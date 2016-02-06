#include <libdf3d/io/Storage.h>

namespace df3d { namespace platform_impl {

class AndroidStorage : public Storage
{
public:
    AndroidStorage(const std::string &filename)
        : Storage(filename)
    {

    }

    virtual void save() override
    {

    }
};

}

Storage* Storage::create(const std::string &filename)
{
    return new platform_impl::AndroidStorage(filename);
}

}

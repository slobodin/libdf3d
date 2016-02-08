#include <libdf3d/io/Storage.h>

#include <libdf3d/utils/JsonUtils.h>

namespace df3d { namespace platform_impl {

class WindowsStorage : public Storage
{
    void saveToFileSystem(const std::string &data) override
    {
        std::ofstream of(m_fileName);
        of << data;
    }

    bool getFromFileSystem(std::string &outStr) override
    {
        std::ifstream ifs(m_fileName);
        if (ifs)
        {
            ifs >> outStr;
            return true;
        }

        return false;
    }

public:
    WindowsStorage(const std::string &filename)
        : Storage(filename)
    {

    }
};

}

Storage* Storage::create(const std::string &filename)
{
    return new platform_impl::WindowsStorage(filename);
}

}

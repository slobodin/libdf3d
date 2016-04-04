#include <libdf3d/io/Storage.h>

#include <libdf3d/utils/JsonUtils.h>

namespace df3d { namespace platform_impl {

class WindowsStorage : public Storage
{
    void saveToFileSystem(const std::string &data) override
    {
        std::ofstream of(m_fileName, std::ios::out | std::ios::binary);
        of.write(data.c_str(), data.size());
    }

    bool getFromFileSystem(std::string &outStr) override
    {
        std::ifstream ifs(m_fileName, std::ios::in | std::ios::binary);
        if (ifs)
        {
            size_t fileSize = 0;
            ifs.seekg(0, std::ios_base::end);
            fileSize = ifs.tellg();
            ifs.seekg(0, std::ios_base::beg);

            outStr.resize(fileSize);
            ifs.read(&outStr[0], fileSize);

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

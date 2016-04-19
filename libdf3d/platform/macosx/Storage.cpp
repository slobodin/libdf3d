#include <libdf3d/io/Storage.h>

namespace df3d { namespace platform_impl {

class MacosxStorage : public Storage
{
    void saveToFileSystem(const uint8_t *data, size_t size) override
    {
        std::ofstream of(m_fileName, std::ios::out | std::ios::binary);
        of.write((const char *)data, size);
    }

    bool getFromFileSystem(uint8_t **data, size_t *size) override
    {
        std::ifstream ifs(m_fileName, std::ios::in | std::ios::binary);
        if (ifs)
        {
            size_t fileSize = 0;
            ifs.seekg(0, std::ios_base::end);
            fileSize = ifs.tellg();
            ifs.seekg(0, std::ios_base::beg);

            *data = new uint8_t[fileSize];
            *size = fileSize;

            ifs.read((char*)*data, fileSize);

            return true;
        }

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

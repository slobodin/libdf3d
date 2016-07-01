#include "PlatformStorage.h"

namespace df3d {

bool PlatformStorage::saveData(const char *id, const std::vector<uint8_t> &data)
{
    std::ofstream of(id, std::ios::out | std::ios::binary);

    of.write((const char *)data.data(), data.size());
    return !of.fail();
}

void PlatformStorage::getData(const char *id, std::vector<uint8_t> &data)
{
    data.clear();

    std::ifstream ifs(id, std::ios::in | std::ios::binary);
    if (ifs)
    {
        size_t fileSize = 0;
        ifs.seekg(0, std::ios_base::end);
        fileSize = static_cast<size_t>(ifs.tellg());
        ifs.seekg(0, std::ios_base::beg);

        data.resize(fileSize);

        ifs.read((char*)&data[0], fileSize);
    }
}

}

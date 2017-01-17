#include "Storage.h"

#include <df3d/lib/JsonUtils.h>
#include <df3d/lib/os/PlatformStorage.h>
#include <df3d/engine/EngineController.h>

namespace df3d
{

struct NullEncryptor : Storage::Encryptor
{
    PodArray<uint8_t> encode(const std::string &jsonData) override
    {
        PodArray<uint8_t> result(MemoryManager::allocDefault());
        result.assign(reinterpret_cast<const uint8_t*>(jsonData.c_str()), jsonData.size());
        return result;
    }

    std::string decode(const PodArray<uint8_t> &input) override
    {
        return std::string(reinterpret_cast<const char*>(input.data(), input.size()));
    }
};

Storage::Storage(const std::string &filename)
    : m_encryptor(make_unique<NullEncryptor>()),
    m_fileName(filename)
{

}

bool Storage::save(const Json::Value &root)
{
    Json::StreamWriterBuilder b;
#ifdef _DEBUG
    b["indentation"] = "  ";
#else
    b["indentation"] = "";
#endif

    auto output = m_encryptor->encode(Json::writeString(b, root));

    return PlatformStorage::saveData(m_fileName.c_str(), output);
}

Json::Value Storage::load()
{
    PodArray<uint8_t> input(MemoryManager::allocDefault());
    PlatformStorage::getData(m_fileName.c_str(), input);

    if (input.size() == 0)
        return {};

    return JsonUtils::fromString(m_encryptor->decode(input));
}

}

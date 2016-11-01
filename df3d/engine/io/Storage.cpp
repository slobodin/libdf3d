#include "Storage.h"

#include <df3d/lib/JsonUtils.h>
#include <df3d/lib/os/PlatformStorage.h>
#include <df3d/engine/EngineController.h>

namespace df3d
{

struct NullEncryptor : Storage::Encryptor
{
    PodArray<uint8_t> encode(const PodArray<uint8_t> &input) override
    {
        return input;
    }

    PodArray<uint8_t> decode(const PodArray<uint8_t> &input) override
    {
        return input;
    }
};

Storage::Storage(const std::string &filename)
    : m_encryptor(make_unique<NullEncryptor>()),
    m_data(Json::ValueType::objectValue),
    m_fileName(filename)
{

}

bool Storage::save()
{
    Json::StreamWriterBuilder b;
#ifdef _DEBUG
    b["indentation"] = "  ";
#else
    b["indentation"] = "";
#endif

    auto str = Json::writeString(b, getData());

    PodArray<uint8_t> input(MemoryManager::allocDefault());
    input.assign(reinterpret_cast<const uint8_t*>(str.data()), str.size());

    auto output = m_encryptor->encode(input);

    return PlatformStorage::saveData(m_fileName.c_str(), output);
}

bool Storage::load()
{
    PodArray<uint8_t> input(MemoryManager::allocDefault());
    PlatformStorage::getData(m_fileName.c_str(), input);

    if (input.size() == 0)
        return false;

    auto output = m_encryptor->decode(input);

    std::string jsonSource;
    jsonSource.assign(output.begin(), output.end());

    auto jsonData = JsonUtils::fromString(jsonSource);
    if (jsonData.isNull())
        return false;

    m_data = std::move(jsonData);

    return true;
}

}

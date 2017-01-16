#include "Storage.h"

#include <df3d/lib/JsonUtils.h>
#include <df3d/lib/os/PlatformStorage.h>
#include <df3d/engine/EngineController.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>

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
    m_fileName(filename)
{

}

bool Storage::save(const rapidjson::Document &root)
{
    rapidjson::StringBuffer buffer;
#ifdef _DEBUG
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
#else
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
#endif
    root.Accept(writer);

    PodArray<uint8_t> input(MemoryManager::allocDefault());
    input.assign(reinterpret_cast<const uint8_t*>(buffer.GetString()), buffer.GetSize());

    auto output = m_encryptor->encode(input);

    return PlatformStorage::saveData(m_fileName.c_str(), output);
}

rapidjson::Document Storage::load()
{
    PodArray<uint8_t> input(MemoryManager::allocDefault());
    PlatformStorage::getData(m_fileName.c_str(), input);

    if (input.size() == 0)
        return {};

    auto output = m_encryptor->decode(input);

    std::string jsonSource;
    jsonSource.assign(output.begin(), output.end());

    return JsonUtils::fromString(jsonSource);
}

}

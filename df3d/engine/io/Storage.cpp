#include "Storage.h"

#include <df3d/lib/JsonUtils.h>
#include <df3d/lib/os/PlatformStorage.h>

namespace df3d
{

struct NullEncryptor : Storage::Encryptor
{
    std::vector<uint8_t> encode(const std::vector<uint8_t> &input) override
    {
        return input;
    }

    std::vector<uint8_t> decode(const std::vector<uint8_t> &input) override
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

    std::vector<uint8_t> input;
    input.assign(str.begin(), str.end());

    auto output = m_encryptor->encode(input);

    PlatformStorage::saveData(m_fileName.c_str(), output);

    return true;
}

bool Storage::load()
{
    std::vector<uint8_t> input;
    PlatformStorage::getData(m_fileName.c_str(), input);

    if (input.size() == 0)
        return false;

    auto output = m_encryptor->decode(input);

    std::string jsonSource;
    jsonSource.assign(output.begin(), output.end());

    auto jsonData = JsonUtils::fromSource(jsonSource);
    if (jsonData.isNull())
        return false;

    m_data = std::move(jsonData);

    return true;
}

}

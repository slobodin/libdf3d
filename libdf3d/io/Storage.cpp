#include "Storage.h"

#include <libdf3d/utils/JsonUtils.h>

namespace df3d
{

struct NullEncryptor : Storage::Encryptor
{
    void encode(uint8_t *data, size_t size) override
    {
        // pass
    }

    void decode(uint8_t *data, size_t size) override
    {
        // pass
    }
};

Storage::Storage(const std::string &filename)
    : m_data(Json::ValueType::objectValue),
    m_fileName(filename),
    m_encryptor(make_unique<NullEncryptor>())
{

}

bool Storage::save()
{
    Json::StreamWriterBuilder b;
    b["indentation"] = "";

    auto str = Json::writeString(b, getData());

    uint8_t *data = (uint8_t*)&str[0];
    m_encryptor->encode(data, str.size());

    saveToFileSystem(data, str.size());

    return true;
}

bool Storage::load()
{
    uint8_t *data = nullptr;
    size_t size = 0;
    if (!getFromFileSystem(&data, &size))
        return false;

    m_encryptor->decode(data, size);

    std::string jsonSource;
    jsonSource.assign((const char *)data, size);

    delete[] data;

    auto jsonData = utils::json::fromSource(jsonSource);
    if (jsonData.isNull())
        return false;

    m_data = std::move(jsonData);

    return true;
}

}

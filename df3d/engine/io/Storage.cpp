#include "Storage.h"

#include <df3d/lib/JsonUtils.h>

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

    saveToFileSystem(&output[0], output.size());

    return true;
}

bool Storage::load()
{
    uint8_t *data = nullptr;
    size_t size = 0;
    if (!getFromFileSystem(&data, &size))
        return false;

    std::vector<uint8_t> input;
    input.assign(data, data + size);

    auto output = m_encryptor->decode(input);

    std::string jsonSource;
    jsonSource.assign(output.begin(), output.end());

    delete[] data;

    auto jsonData = utils::json::fromSource(jsonSource);
    if (jsonData.isNull())
        return false;

    m_data = std::move(jsonData);

    return true;
}

}

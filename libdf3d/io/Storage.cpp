#include "Storage.h"

#include <libdf3d/utils/JsonUtils.h>

namespace df3d
{

struct NullEncryptor : Storage::Encryptor
{
    std::string encode(const std::string &data) override
    {
        return data;
    }

    std::string decode(const std::string &data) override
    {
        return data;
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
    b["indentation"] ="";

    auto str = Json::writeString(b, getData());
    saveToFileSystem(m_encryptor->encode(str));

    return true;
}

bool Storage::load()
{
    std::string strData;
    if (!getFromFileSystem(strData))
        return false;

    auto jsonData = utils::json::fromSource(m_encryptor->decode(strData));
    if (jsonData.isNull())
        return false;

    m_data = std::move(jsonData);

    return true;
}

}

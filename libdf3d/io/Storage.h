#pragma once

namespace df3d {

class DF3D_DLL Storage : utils::NonCopyable
{
protected:
    Json::Value m_data;
    std::string m_fileName;

    Storage(const std::string &filename) 
        : m_data(Json::ValueType::objectValue),
        m_fileName(filename) 
    { }

public:
    static Storage *create(const std::string &filename);
    virtual ~Storage() { }

    const Json::Value& data() const { return m_data; }
    Json::Value& data() { return m_data; }

    // TODO:
    // Serialize/deserialize to bytearray.

    virtual void save() = 0;
};

}

#pragma once

namespace df3d {

class DF3D_DLL Storage : utils::NonCopyable
{
public:
    struct Encryptor
    {
        virtual ~Encryptor() = default;

        virtual std::string encode(const std::string &data) = 0;
        virtual std::string decode(const std::string &data) = 0;
    };

private:
    unique_ptr<Encryptor> m_encryptor;

protected:
    Json::Value m_data;
    std::string m_fileName;

    Storage(const std::string &filename);

    virtual void saveToFileSystem(const std::string &data) = 0;
    virtual bool getFromFileSystem(std::string &outStr) = 0;

public:
    static Storage *create(const std::string &filename);
    virtual ~Storage() { }

    void setEncryptor(unique_ptr<Encryptor> e) { m_encryptor = std::move(e); }

    const Json::Value& getData() const { return m_data; }
    Json::Value& getData() { return m_data; }

    bool save();
    bool load();
};

}

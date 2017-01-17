#pragma once

namespace df3d {

class Storage : NonCopyable
{
public:
    struct Encryptor
    {
        virtual ~Encryptor() = default;

        virtual PodArray<uint8_t> encode(const std::string &jsonData) = 0;
        virtual std::string decode(const PodArray<uint8_t> &input) = 0;
    };

private:
    unique_ptr<Encryptor> m_encryptor;

    std::string m_fileName;

public:
    Storage(const std::string &filename);
    virtual ~Storage() { }

    void setEncryptor(unique_ptr<Encryptor> e) { m_encryptor = std::move(e); }

    bool save(const Json::Value &root);
    Json::Value load();
};

}

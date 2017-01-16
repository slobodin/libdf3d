#pragma once

namespace df3d {

class Storage : NonCopyable
{
public:
    struct Encryptor
    {
        virtual ~Encryptor() = default;

        virtual PodArray<uint8_t> encode(const PodArray<uint8_t> &input) = 0;
        virtual PodArray<uint8_t> decode(const PodArray<uint8_t> &input) = 0;
    };

private:
    unique_ptr<Encryptor> m_encryptor;

    std::string m_fileName;

public:
    Storage(const std::string &filename);
    virtual ~Storage() { }

    void setEncryptor(unique_ptr<Encryptor> e) { m_encryptor = std::move(e); }

    bool save(const rapidjson::Document &root);
    rapidjson::Document load();
};

}

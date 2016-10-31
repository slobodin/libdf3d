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

protected:
    Json::Value m_data;
    std::string m_fileName;

public:
    Storage(const std::string &filename);
    virtual ~Storage() { }

    void setEncryptor(unique_ptr<Encryptor> e) { m_encryptor = std::move(e); }

    const Json::Value& getData() const { return m_data; }
    Json::Value& getData() { return m_data; }

    bool save();
    bool load();
};

}

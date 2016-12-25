#pragma once

namespace df3d {

class Id
{
#ifdef _DEBUG
    std::string m_debugStr;
#endif

public:
    uint32_t m_id;

    Id();
    explicit Id(const char *str);
    ~Id();

    std::string toString() const;
    bool empty() const;

    bool operator== (const Id &other) const { return m_id == other.m_id; }
    bool operator!= (const Id &other) const { return m_id != other.m_id; }
};

}

namespace std {

template <>
struct hash<df3d::Id>
{
    std::size_t operator()(const df3d::Id &id) const
    {
        return id.m_id;
    }
};

}

#define DFID(x) df3d::Id(x)

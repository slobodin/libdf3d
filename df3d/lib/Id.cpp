#include "Id.h"

namespace df3d {

#ifdef _DEBUG
struct IdsTable
{
    std::recursive_mutex m_lock;
    std::unordered_map<uint32_t, std::string> m_ids;

    void add(uint32_t id, const std::string &str)
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);

        auto insertRes = m_ids.insert({ id, str });
        if (!insertRes.second)
            DF3D_ASSERT_MESS(insertRes.first->second == str, "Id collision!");
    }

    void remove(uint32_t id)
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);

        m_ids.erase(id);
    }

    static IdsTable& instance()
    {
        static IdsTable table;
        return table;
    }
};
#endif

inline uint32_t CalcFNV(const char *str)
{
    uint32_t hash = 2166136261u;
    while (*str)
    {
        hash ^= *str++;
        hash *= 16777619u;
    }

    return hash;
}

Id::Id()
    : Id("")
{

}

Id::Id(const char *str)
    : m_id(CalcFNV(str))
{
#ifdef _DEBUG
    m_debugStr = str;
    IdsTable::instance().add(m_id, m_debugStr);
#endif
}

Id::~Id()
{
#ifdef _DEBUG
    IdsTable::instance().remove(m_id);
#endif
}

std::string Id::toString() const
{
#ifdef _DEBUG
    return m_debugStr;
#else
    return std::to_string(m_id);
#endif
}

bool Id::empty() const
{
    return m_id == CalcFNV("");
}

}

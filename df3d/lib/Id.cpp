#include "Id.h"

#include <df3d/lib/Utils.h>

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

    const std::string* lookup(uint32_t id)
    {
        auto found = m_ids.find(id);
        if (found != m_ids.end())
            return &found->second;
        return nullptr;
    }

    static IdsTable& instance()
    {
        static IdsTable table;
        return table;
    }
};
#endif

// FIXME: should be the same as in TB because looking up TBLanguage by Id.
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
    IdsTable::instance().add(m_id, str);
#endif
}

Id::~Id()
{

}

std::string Id::toString() const
{
#ifdef _DEBUG
    if (auto found = IdsTable::instance().lookup(m_id))
        return *found;
    DF3D_ASSERT(false);
    return {};
#else
    return std::to_string(m_id);
#endif
}

bool Id::empty() const
{
    return m_id == CalcFNV("");
}

Id Id::fromString(const std::string &str)
{
#ifdef _DEBUG
    return Id(str.c_str());
#else
    Id res;
    res.m_id = df3d::utils::from_string<uint32_t>(str);
    return res;
#endif
}

}

#pragma once

#include "Any.h"
#include "Utils.h"

namespace df3d {

//! Type erasure key-value pair container.
/*!
 * \brief Prefer to use only with POD types.
 */
class Dict
{
    std::unordered_map<std::string, Any> m_kvPairs;

public:
    Dict();
    ~Dict();

    template<typename T>
    void set(const std::string &key, const T &val)
    {
        m_kvPairs[key] = val;
    }

    template<typename T>
    T get(const std::string &key) const
    {
        auto found = m_kvPairs.find(key);
        if (found == m_kvPairs.end())
            return T();

        return found->second.get<T>();
    }

    bool contains(const std::string &key)
    {
        return utils::contains_key(m_kvPairs, key);
    }

    void erase(const std::string &key)
    {
        m_kvPairs.erase(key);
    }

    void clear()
    {
        m_kvPairs.clear();
    }
};

}

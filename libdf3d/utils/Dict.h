#pragma once

#include <boost/any.hpp>

namespace df3d { namespace utils {

//! Type erasure key-value pair container.
/*!
 * \brief Prefer to use only with POD types.
 */
class DF3D_DLL Dict
{
    std::unordered_map<std::string, boost::any> m_kvPairs;

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

        try
        {
            return boost::any_cast<T>(found->second);
        }
        catch(boost::bad_any_cast &)
        {
            return T();
        }
    }
};

} }

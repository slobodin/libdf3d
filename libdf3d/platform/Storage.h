#pragma once

namespace df3d { namespace platform {

class DF3D_DLL Storage : boost::noncopyable
{
protected:
    using Entry = boost::variant<int, int64_t, double, bool, std::string>;

    std::unordered_map<std::string, Entry> m_entries;
    std::string m_fileName;

    Storage(const char *filename) : m_fileName(filename) { }

public:
    static Storage *create(const char *filename);
    virtual ~Storage() { }

    template<typename T>
    void set(const char *key, const T &val)
    {
        m_entries[key] = val;
    }

    template<typename T>
    T get(const char *key) const
    {
        auto it = m_entries.find(key);
        if (it == m_entries.end())
            return T();

        try
        {
            return boost::get<T>(it->second);
        }
        catch (boost::bad_get &) 
        {
            base::glog << "Storage: bad get" << base::logwarn;
        }

        return T();
    }

    bool contains(const char *key) const
    {
        return m_entries.find(key) != m_entries.end();
    }

    bool erase(const char *key)
    {
        return m_entries.erase(key) != 0;
    }

    void clear()
    {
        m_entries.clear();
    }

    virtual void save() = 0;
};

} }

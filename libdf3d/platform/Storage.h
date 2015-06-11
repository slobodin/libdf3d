#pragma once

namespace df3d { namespace platform {

class DF3D_DLL Storage : boost::noncopyable
{
protected:
    using Entry = boost::variant<int32_t,
                                 int64_t,
                                 float,
                                 double,
                                 bool,
                                 std::string>;

    std::unordered_map<std::string, Entry> m_entries;

    Storage() { }
    virtual ~Storage() { }

public:
    static Storage *getInstance();

    template<typename T>
    T &operator[](const char *key);

    template<typename T>
    const T &operator[](const char *key) const;

    bool contains(const char *key) const;

    bool erase(const char *key);
    void clear();

    void save();


};

} }

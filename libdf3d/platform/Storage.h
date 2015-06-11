#pragma once

namespace df3d { namespace platform {

class DF3D_DLL Storage : boost::noncopyable
{
public:
    static Storage *getInstance();

    template<typename T>
    T &operator[](const char *key);

protected:
    Storage() { }
    virtual ~Storage() { }
};

} }

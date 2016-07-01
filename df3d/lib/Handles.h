#pragma once

namespace df3d {

#define DF3D_MAKE_SHORT_HANDLE(name) struct name { \
    int16_t id; \
    name(int16_t id = -1) : id(id) { } \
    bool valid() const { return id != -1; } \
    void invalidate() { id = -1; } \
    bool operator<(const name &other) const { return id < other.id; } };

#define DF3D_MAKE_HANDLE(name) struct name { \
    int32_t id; \
    name(int32_t id = -1) : id(id) { } \
    bool valid() const { return id != -1; } \
    void invalidate() { id = -1; } \
    bool operator<(const name &other) const { return id < other.id; } };

class DF3D_DLL HandleBag
{
public:
    HandleBag();
    ~HandleBag();

    void getNew();
    void release();
};

}

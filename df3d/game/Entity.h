#pragma once

#include <df3d/lib/Handles.h>

namespace df3d {

DF3D_MAKE_HANDLE(Entity)

}


namespace std {

template <>
struct hash<df3d::Entity>
{
    std::size_t operator()(const df3d::Entity &e) const
    {
        auto id = e.handle.getID();
        return std::hash<decltype(id)>()(id);
    }
};

}

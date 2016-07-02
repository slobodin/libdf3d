#pragma once

#include <df3d/lib/Handles.h>

namespace df3d {

DF3D_MAKE_HANDLE(EntityHandle)
using Entity = EntityHandle;

DF3D_MAKE_HANDLE(ComponentInstanceHandle)
using ComponentInstance = ComponentInstanceHandle;

}

namespace std {

template <>
struct hash<df3d::Entity>
{
    std::size_t operator()(const df3d::Entity& e) const
    {
        return std::hash<decltype(e.id)>()(e.id);
    }
};

template <>
struct hash<df3d::ComponentInstance>
{
    std::size_t operator()(const df3d::ComponentInstance& e) const
    {
        return std::hash<decltype(e.id)>()(e.id);
    }
};

}

#pragma once

#include <third-party/bullet/src/LinearMath/btVector3.h>

namespace df3d {

struct MeshResourceData;

struct ConvexHull
{
    std::vector<btVector3> m_vertices;

    void constructFromGeometry(const MeshResourceData &resource, Allocator &alloc);
};

}

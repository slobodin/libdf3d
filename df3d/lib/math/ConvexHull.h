#pragma once

#include <third-party/bullet/src/LinearMath/btVector3.h>
#include <vector>

namespace df3d {

struct MeshResourceData;
class Allocator;

struct ConvexHull
{
    std::vector<btVector3> m_vertices;

    void constructFromGeometry(const MeshResourceData &resource, Allocator &alloc);
};

}

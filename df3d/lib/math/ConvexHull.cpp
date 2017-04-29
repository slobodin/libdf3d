#include "ConvexHull.h"

#include <df3d/engine/resources/MeshResource.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

namespace df3d {

void ConvexHull::constructFromGeometry(const MeshResourceData &resource, Allocator &alloc)
{
    auto tempHull = MAKE_NEW(alloc, btConvexHullShape)();

    // Compute the volume.
    for (auto submesh : resource.parts)
    {
        const auto &vertexData = submesh->vertexData;

        for (size_t i = 0; i < vertexData.getVerticesCount(); i++)
        {
            auto &vdata = const_cast<VertexData&>(vertexData);
            auto v = (glm::vec3*)vdata.getVertexAttribute(i, VertexFormat::POSITION);

            tempHull->addPoint({ v->x, v->y, v->z }, false);
        }

        tempHull->recalcLocalAabb();
    }

    auto convexHull = MAKE_NEW(alloc, btShapeHull)(tempHull);
    convexHull->buildHull(tempHull->getMargin());

    auto vertices = convexHull->getVertexPointer();
    for (int i = 0; i < convexHull->numVertices(); i++)
    {
        auto p = vertices[i];

        m_vertices.push_back({ p.x(), p.y(), p.z() });
    }

    MAKE_DELETE(alloc, convexHull);
    MAKE_DELETE(alloc, tempHull);
}

}

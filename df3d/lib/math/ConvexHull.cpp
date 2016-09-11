#include "ConvexHull.h"

#include <df3d/engine/render/MeshData.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

namespace df3d {

ConvexHull::ConvexHull(Allocator *allocator)
    : m_vertices(allocator)
{

}

ConvexHull::~ConvexHull()
{

}

void ConvexHull::reset()
{
    m_vertices.clear();
}

void ConvexHull::updateBounds(const glm::vec3 &point)
{

}

bool ConvexHull::isValid() const
{
    return m_vertices.size() > 0;
}

void ConvexHull::constructFromGeometry(const std::vector<SubMesh> &submeshes)
{
    reset();

    auto tempHull = new btConvexHullShape();

    // Compute the volume.
    for (const auto &submesh : submeshes)
    {
        const auto &vertexData = submesh.vertexData;

        for (size_t i = 0; i < vertexData.getVerticesCount(); i++)
        {
            auto &vdata = const_cast<VertexData&>(vertexData);
            auto v = (glm::vec3*)vdata.getVertexAttribute(i, VertexFormat::POSITION);

            tempHull->addPoint({ v->x, v->y, v->z }, false);
        }

        tempHull->recalcLocalAabb();
    }

    auto convexHull = new btShapeHull(tempHull);
    convexHull->buildHull(tempHull->getMargin());

    auto vertices = convexHull->getVertexPointer();
    for (int i = 0; i < convexHull->numVertices(); i++)
    {
        auto p = vertices[i];

        m_vertices.push_back({ p.x(), p.y(), p.z() });
    }

    delete convexHull;
    delete tempHull;
}

}

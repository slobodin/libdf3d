#include "ConvexHull.h"

#include <libdf3d/render/MeshData.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

namespace df3d {

ConvexHull::ConvexHull()
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
        const auto &vertexData = submesh.getVertexData();

        // Some sanity checks.
        if (!vertexData.getFormat().hasAttribute(VertexFormat::POSITION_3))
            continue;

        for (size_t i = 0; i < vertexData.getVerticesCount(); i++)
        {
            auto v = const_cast<VertexData&>(vertexData).getVertex(i);      // sorry, but it's really const...
            glm::vec3 p;
            v.getPosition(&p);

            tempHull->addPoint({ p.x, p.y, p.z }, false);
        }

        tempHull->recalcLocalAabb();
    }

    auto convexHull = new btShapeHull(tempHull);
    convexHull->buildHull(tempHull->getMargin());

    auto vertices = convexHull->getVertexPointer();
    for (int i = 0; i < convexHull->numVertices(); i++)
    {
        auto p = convexHull->getVertexPointer()[i];

        m_vertices.push_back({ p.x(), p.y(), p.z() });
    }

    delete convexHull;
    delete tempHull;
}

}

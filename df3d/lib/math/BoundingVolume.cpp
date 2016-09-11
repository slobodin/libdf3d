#include "BoundingVolume.h"

#include <df3d/engine/render/MeshData.h>

namespace df3d {

void BoundingVolume::constructFromGeometry(const std::vector<SubMesh> &submeshes)
{
    reset();

    // Compute volume.
    for (const auto &submesh : submeshes)
    {
        const auto &vertexData = submesh.vertexData;

        for (size_t i = 0; i < vertexData.getVerticesCount(); i++)
        {
            auto &vdata = const_cast<VertexData&>(vertexData);
            auto v = (glm::vec3*)vdata.getVertexAttribute(i, VertexFormat::POSITION);

            if (v)
                updateBounds(*v);
        }
    }
}

}

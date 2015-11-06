#include "BoundingVolume.h"

#include <render/MeshData.h>

namespace df3d {

void BoundingVolume::constructFromGeometry(const std::vector<SubMesh> &submeshes)
{
    reset();

    // Compute volume.
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

            updateBounds(p);
        }
    }
}

}

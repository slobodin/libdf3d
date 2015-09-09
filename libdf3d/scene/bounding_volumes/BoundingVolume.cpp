#include "df3d_pch.h"
#include "BoundingVolume.h"

#include <render/MeshData.h>

namespace df3d { namespace scene {

void BoundingVolume::constructFromGeometry(const std::vector<render::SubMesh> &submeshes)
{
    reset();

    // Compute volume.
    for (const auto &submesh : submeshes)
    {
        auto vertexData = submesh.getVertexData();
        const auto vertexFormat = submesh.getVertexFormat();

        // Some sanity checks.
        auto positionComponent = vertexFormat.getComponent(render::VertexComponent::POSITION);
        if (!positionComponent || positionComponent->getCount() != 3)
            continue;

        auto offsetToPosition = vertexFormat.getOffsetTo(render::VertexComponent::POSITION) / sizeof(float);
        auto stride = vertexFormat.getVertexSize() / sizeof(float);
        auto vertexCount = submesh.getVerticesCount();

        for (size_t i = 0; i < vertexCount; i++)
        {
            auto offset = i * stride + offsetToPosition;
            
            glm::vec3 p(vertexData[offset + 0], vertexData[offset + 1], vertexData[offset + 2]);

            updateBounds(p);
        }
    }
}

} }

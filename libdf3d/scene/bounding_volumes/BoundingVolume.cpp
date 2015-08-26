#include "df3d_pch.h"
#include "BoundingVolume.h"

#include <render/MeshData.h>
#include <render/SubMesh.h>
#include <render/VertexIndexBuffer.h>

namespace df3d { namespace scene {

void BoundingVolume::constructFromGeometry(shared_ptr<render::MeshData> geometry)
{
    if (!geometry->isValid())
        return;

    reset();

    // Compute volume.
    const auto &submeshes = geometry->getSubMeshes();
    for (const auto &submesh : submeshes)
    {
        auto vb = submesh->getVertexBuffer();
        auto vertexData = vb->getVertexData();

        // Some sanity checks.
        auto positionComponent = vb->getFormat().getComponent(render::VertexComponent::POSITION);
        if (!positionComponent || positionComponent->getCount() != 3)
            continue;

        auto offsetToPosition = vb->getFormat().getOffsetTo(render::VertexComponent::POSITION) / sizeof(float);
        auto stride = vb->getFormat().getVertexSize() / sizeof(float);
        auto vertexCount = vb->getVerticesCount();

        for (size_t i = 0; i < vertexCount; i++)
        {
            auto offset = i * stride + offsetToPosition;
            
            glm::vec3 p(vertexData[offset + 0], vertexData[offset + 1], vertexData[offset + 2]);

            updateBounds(p);
        }
    }
}

} }
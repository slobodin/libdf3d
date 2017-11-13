#pragma once

#include <df3d/engine/render/RenderCommon.h>

namespace df3d {

class RenderPass;

//! Render batch.
class RenderOperation
{
public:
    glm::mat4 worldTransform;

    RenderPass *passProps = nullptr;
    Topology topology = Topology::TRIANGLES;

    VertexBufferHandle vertexBuffer;
    IndexBufferHandle indexBuffer;

    //! Starting vertex when binding vertex buffer.
    uint32_t startVertex = 0;
    //! Number of elements to draw (i.e., number of vertices or number of indices if using index buffer)
    uint32_t numberOfElements = 0;

    float z = 0.0f;
};

}

#pragma once

#include <libdf3d/render/RenderCommon.h>

namespace df3d {

class RenderPass;

//! Render batch.
class RenderOperation
{
public:
    RopType type = RopType::TRIANGLES;
    glm::mat4 worldTransform;

    VertexBufferDescriptor vertexBuffer;
    IndexBufferDescriptor indexBuffer;
    RenderPass *passProps = nullptr;
    size_t numberOfElements = 0;

    RenderOperation() = default;
    ~RenderOperation() = default;
};

class RenderOperation2D : public RenderOperation
{
public:
    float z = 0.0f;
};

}

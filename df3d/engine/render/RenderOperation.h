#pragma once

#include <df3d/engine/render/RenderCommon.h>

namespace df3d {

class RenderPass;

//! Render batch.
class RenderOperation
{
public:
#ifdef _DEBUG
    std::string debugID;
#endif
    Topology topology = Topology::TRIANGLES;
    glm::mat4 worldTransform;

    VertexBufferHandle vertexBuffer;
    IndexBufferHandle indexBuffer;
    RenderPass *passProps = nullptr;
    size_t numberOfElements = 0;
    size_t vertexBufferOffset = 0;

    RenderOperation() = default;
    ~RenderOperation() = default;
};

class RenderOperation2D : public RenderOperation
{
public:
    float z = 0.0f;
};

}

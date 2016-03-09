#pragma once

#include <libdf3d/render/RenderCommon.h>

namespace df3d {

class RenderPass;

//! Render batch.
class RenderOperation
{
public:
    enum class Type
    {
        LINES,
        TRIANGLES,
        LINE_STRIP
    };

    Type type = Type::TRIANGLES;
    glm::mat4 worldTransform;

    VertexBufferDescriptor vertexBuffer;
    IndexBufferDescriptor indexBuffer;
    RenderPass *passProps = nullptr;

    RenderOperation() = default;
    ~RenderOperation() = default;
};

class RenderOperation2D : public RenderOperation
{
public:
    float z = 0.0f;
};

}

#pragma once

namespace df3d {

class VertexBuffer;
class IndexBuffer;
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

    IndexBuffer *indexData = nullptr;
    VertexBuffer *vertexData = nullptr;
    RenderPass *passProps = nullptr;

    RenderOperation() = default;
    ~RenderOperation() = default;

    bool isEmpty() const;
};

class RenderOperation2D : public RenderOperation
{
public:
    float z = 0.0f;
};

}

#pragma once

namespace df3d { namespace render {

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
        TRIANGLES
    };

    Type type = Type::TRIANGLES;

    glm::mat4 worldTransform;

    // TODO_REFACTO: make this just pointers.

    shared_ptr<IndexBuffer> indexData = nullptr;
    shared_ptr<VertexBuffer> vertexData = nullptr;
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

} }

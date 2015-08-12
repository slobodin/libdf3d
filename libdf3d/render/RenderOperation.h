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

    shared_ptr<IndexBuffer> indexData;
    shared_ptr<VertexBuffer> vertexData;
    shared_ptr<RenderPass> passProps;

    RenderOperation();
    ~RenderOperation();

    bool isEmpty() const;
};

class RenderOperation2D : public RenderOperation
{
public:
    float z = 0.0f;
};

} }

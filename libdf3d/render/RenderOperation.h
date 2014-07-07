#pragma once

namespace df3d { namespace render {

class VertexBuffer;
class IndexBuffer;
class RenderPass;

//! Render batch.
class RenderOperation
{
public:
    enum Type
    {
        LINE_LIST,
        TRIANGLE_LIST
    };

    Type type = TRIANGLE_LIST;

    glm::mat4 worldTransform;

    shared_ptr<IndexBuffer> indexData;
    shared_ptr<VertexBuffer> vertexData;
    shared_ptr<RenderPass> passProps;

    RenderOperation();
    ~RenderOperation();
};

} }
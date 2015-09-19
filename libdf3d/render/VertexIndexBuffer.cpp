#include "VertexIndexBuffer.h"

#include "OpenGLCommon.h"

namespace df3d {
namespace render {

GLenum getGLUsageType(GpuBufferUsageType t)
{
    switch (t)
    {
    case GpuBufferUsageType::STATIC:
        return GL_STATIC_DRAW;
    case GpuBufferUsageType::DYNAMIC:
        return GL_DYNAMIC_DRAW;
    case GpuBufferUsageType::STREAM:
        return GL_STREAM_DRAW;
    default:
        break;
    }

    return GL_INVALID_ENUM;
}

VertexBuffer::VertexBuffer(const VertexFormat &format)
    : m_format(format)
{
    glGenBuffers(1, &m_glId);

    assert(m_glId);     // Silly assert, but let it be.
}

VertexBuffer::~VertexBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &m_glId);
}

void VertexBuffer::alloc(size_t verticesCount, const void *data, GpuBufferUsageType usage)
{
    assert(verticesCount > 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_glId);
    glBufferData(GL_ARRAY_BUFFER, verticesCount * m_format.getVertexSize(), data, getGLUsageType(usage));

    m_verticesUsed = verticesCount;
    m_sizeInBytes = verticesCount * m_format.getVertexSize();
}

void VertexBuffer::alloc(const VertexData &data, GpuBufferUsageType usage)
{
    assert(data.getFormat() == m_format);

    alloc(data.getVerticesCount(), data.getRawData(), usage);
}

void VertexBuffer::update(size_t verticesCount, const void *data)
{
    auto bytesUpdating = verticesCount * m_format.getVertexSize();

    assert(bytesUpdating <= m_sizeInBytes);

    glBindBuffer(GL_ARRAY_BUFFER, m_glId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, bytesUpdating, data);
}

void VertexBuffer::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_glId);

    m_format.enableGLAttributes();
}

void VertexBuffer::unbind()
{
    m_format.disableGLAttributes();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

IndexBuffer::IndexBuffer()
{
    glGenBuffers(1, &m_glId);

    assert(m_glId);
}

IndexBuffer::~IndexBuffer()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &m_glId);
}

void IndexBuffer::alloc(size_t indicesCount, const void *data, GpuBufferUsageType usage)
{
    assert(indicesCount > 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(INDICES_TYPE), data, getGLUsageType(usage));

    m_indicesUsed = indicesCount;
    m_sizeInBytes = indicesCount * sizeof(INDICES_TYPE);
}

void IndexBuffer::update(size_t indicesCount, const void *data)
{
    auto bytesUpdating = indicesCount * sizeof(INDICES_TYPE);

    assert(bytesUpdating <= m_sizeInBytes);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glId);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bytesUpdating, data);
}

void IndexBuffer::bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glId);
}

void IndexBuffer::unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

shared_ptr<VertexBuffer> createQuad(const VertexFormat &vf, float x, float y, float w, float h, GpuBufferUsageType usage)
{
    // TODO_REFACTO remove this shit!!!

    float w2 = w / 2.0f;
    float h2 = h / 2.0f;

    float quad_pos[][2] = 
    {
        { x - w2, y - h2 },
        { x + w2, y - h2 },
        { x + w2, y + h2 },
        { x + w2, y + h2 },
        { x - w2, y + h2 },
        { x - w2, y - h2 }
    };
    float quad_uv[][2] = 
    {
        { 0.0, 0.0 },
        { 1.0, 0.0 },
        { 1.0, 1.0 },
        { 1.0, 1.0 },
        { 0.0, 1.0 },
        { 0.0, 0.0 }
    };

    VertexData vertexData(vf);

    for (int i = 0; i < 6; i++)
    {
        auto v = vertexData.getNextVertex();

        v.setPosition({ quad_pos[i][0], quad_pos[i][1], 0.0f });
        v.setTx({ quad_uv[i][0], quad_uv[i][1] });
    }

    auto result = make_shared<VertexBuffer>(vf);
    result->alloc(vertexData, usage);

    return result;
}

shared_ptr<VertexBuffer> createQuad2(const VertexFormat &vf, float x, float y, float w, float h, GpuBufferUsageType usage)
{
    // TODO_REFACTO remove this shit!!!

    float w2 = w / 2.0f;
    float h2 = h / 2.0f;

    float quad_pos[][2] =
    {
        { x - w2, y - h2 },
        { x + w2, y - h2 },
        { x + w2, y + h2 },
        { x + w2, y + h2 },
        { x - w2, y + h2 },
        { x - w2, y - h2 }
    };
    float quad_uv[][2] =
    {
        { 0.0, 0.0 },
        { 1.0, 0.0 },
        { 1.0, 1.0 },
        { 1.0, 1.0 },
        { 0.0, 1.0 },
        { 0.0, 0.0 }
    };

    VertexData vertexData(vf);

    for (int i = 0; i < 6; i++)
    {
        auto v = vertexData.getNextVertex();

        v.setPosition({ quad_pos[i][0], quad_pos[i][1], 0.0f });
        v.setTx({ quad_uv[i][0], quad_uv[i][1] });
        v.setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    }

    auto result = make_shared<VertexBuffer>(vf);
    result->alloc(vertexData, usage);

    return result;
}

} }

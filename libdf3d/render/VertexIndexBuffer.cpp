#include "df3d_pch.h"
#include "VertexIndexBuffer.h"

#include "OpenGLCommon.h"

namespace df3d { namespace render {

GLenum getGLUsageType(GpuBufferUsageType t)
{
    switch (t)
    {
    case GB_USAGE_STATIC:
        return GL_STATIC_DRAW;
    case GB_USAGE_DYNAMIC:
        return GL_DYNAMIC_DRAW;
    case GB_USAGE_STREAM:
        return GL_STREAM_DRAW;
    default:
        break;
    }

    return GL_INVALID_ENUM;
}

void GpuBuffer::setUsageType(GpuBufferUsageType newType)
{
    m_usageType = newType;
}

void GpuBuffer::setDirty()
{
    m_dirty = true;
}

void GpuBuffer::setElementsUsed(size_t elementsCount)
{
    m_elementsUsed = elementsCount;
}

GpuBufferUsageType GpuBuffer::getUsageType() const
{
    return m_usageType;
}

void VertexBuffer::recreateHardwareBuffer()
{
    if (m_glBufferId == 0)
        glGenBuffers(1, &m_glBufferId);

    glBindBuffer(GL_ARRAY_BUFFER, m_glBufferId);

    unsigned int vbSize = m_verticesCount * m_vertexFormat.getVertexSize();
    if (vbSize > 0)
        glBufferData(GL_ARRAY_BUFFER, vbSize, getVertexData(), getGLUsageType(m_usageType));

    m_dirty = false;
    m_cached = true;
    m_gpuBufferSize = vbSize;
}

void VertexBuffer::updateHardwareBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_glBufferId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, getElementsUsed() * m_vertexFormat.getVertexSize(), getVertexData());

    m_dirty = false;
}

VertexBuffer::VertexBuffer(const VertexFormat &vf)
    : m_vertexFormat(vf)
{
}

VertexBuffer::~VertexBuffer()
{
    if (m_glBufferId == 0)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &m_glBufferId);
}

void VertexBuffer::bind()
{
    if (!m_cached)
        recreateHardwareBuffer();

    // Sanity check.
    if (m_glBufferId == 0)
        return;

    if (m_dirty)
    {
        unsigned int vbSize = m_verticesCount * m_vertexFormat.getVertexSize();

        if (vbSize > m_gpuBufferSize)
            recreateHardwareBuffer();
        else
            updateHardwareBuffer();
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_glBufferId);

    m_vertexFormat.enableGLAttributes();
}

void VertexBuffer::unbind()
{
    m_vertexFormat.disableGLAttributes();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::resize(size_t vertexCount)
{
    m_vertexData.resize(m_vertexFormat.getVertexSize() * vertexCount / sizeof(float));
    m_verticesCount = vertexCount;
}

void VertexBuffer::clear()
{
    m_vertexData.clear();
    m_verticesCount = 0;
}

void VertexBuffer::appendVertexData(const float *source, size_t vertexCount)
{
    // FIXME:
    // Don't use back_inserter.
    std::copy(source, source + m_vertexFormat.getVertexSize() * vertexCount / sizeof(float), std::back_inserter(m_vertexData));

    m_verticesCount += vertexCount;
}

size_t VertexBuffer::getElementsUsed() const
{
    if (m_elementsUsed == -1)
        return m_verticesCount;
    return m_elementsUsed;
}

void IndexBuffer::recreateHardwareBuffer()
{
    if (m_glBufferId == 0)
        glGenBuffers(1, &m_glBufferId);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glBufferId);

    unsigned int vbIdxSize = m_indices.size() * sizeof(INDICES_TYPE);
    if (vbIdxSize > 0)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vbIdxSize, getRawIndices(), getGLUsageType(m_usageType));

    m_dirty = false;
    m_cached = true;
    m_gpuBufferSize = vbIdxSize;
}

void IndexBuffer::updateHardwareBuffer()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glBufferId);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, getElementsUsed() * sizeof(INDICES_TYPE), getRawIndices());

    m_dirty = false;
}

IndexBuffer::IndexBuffer()
{
    m_indices.reserve(128);
}

IndexBuffer::~IndexBuffer()
{
    if (m_glBufferId == 0)
        return;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &m_glBufferId);
}

void IndexBuffer::appendIndices(const IndexArray &indices)
{
    if (&indices == &m_indices)
        return;

    m_indices.reserve(m_indices.size() + indices.size());
    std::copy(indices.begin(), indices.end(), std::back_inserter(m_indices));
}

void IndexBuffer::bind()
{
    if (!m_cached)
        recreateHardwareBuffer();

    // Sanity check.
    if (m_glBufferId == 0)
        return;

    if (m_dirty)
    {
        unsigned int vbIdxSize = m_indices.size() * sizeof(INDICES_TYPE);

        if (vbIdxSize > m_gpuBufferSize)
            recreateHardwareBuffer();
        else
            updateHardwareBuffer();
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glBufferId);
}

void IndexBuffer::unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

size_t IndexBuffer::getElementsUsed() const
{
    if (m_elementsUsed == -1)
        return m_indices.size();
    return m_elementsUsed;
}

shared_ptr<VertexBuffer> createQuad(const VertexFormat &vf, float x, float y, float w, float h)
{
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

    auto result = make_shared<VertexBuffer>(vf);

    for (int i = 0; i < 6; i++)
    {
        render::Vertex_3p2tx v;
        v.p.x = quad_pos[i][0];
        v.p.y = quad_pos[i][1];
        v.tx.x = quad_uv[i][0];
        v.tx.y = quad_uv[i][1];

        result->appendVertexData((const float *)&v, 1);
    }

    return result;
}

} }
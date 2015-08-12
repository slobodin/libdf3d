#pragma once

#include "Vertex.h"

namespace df3d { namespace render {

class Material;

// FIXME:
// Make setting.
#define INDICES_16_BIT uint16_t
#define INDICES_32_BIT uint32_t

//! Hint to graphics backend as to how a buffer's data will be accessed.
enum class GpuBufferUsageType
{
    STATIC,     /*!< The data store contents will be modified once and used many times. */
    DYNAMIC,    /*!< The data store contents will be modified repeatedly and used many times. */
    STREAM      /*!< The data store contents will be modified once and used at most a few times. */
};

using INDICES_TYPE = INDICES_32_BIT;
using IndexArray = std::vector<INDICES_TYPE>;

class GpuBuffer : utils::NonCopyable
{
protected:
    GpuBufferUsageType m_usageType = GpuBufferUsageType::STATIC;

    unsigned int m_glBufferId = 0;

    bool m_cached = false;
    bool m_dirty = false;

    int m_elementsUsed = -1;
    size_t m_gpuBufferSize = 0;     // bytes

public:
    //! Sets usage hint.
    void setUsageType(GpuBufferUsageType newType);
    //! Reloads whole buffer from its local data storage when the next bind occurs.
    void setDirty();
    //! Sets count of used buffer elements, i.e. it's possible to draw or update only part of a buffer.
    void setElementsUsed(size_t elementsCount);

    GpuBufferUsageType getUsageType() const;

    // TODO:
    // Buffer lock/unlock via glMapBuffer
};

//! Gpu vertex buffer representation.
class VertexBuffer : public GpuBuffer
{
    VertexFormat m_vertexFormat;

    std::vector<float> m_vertexData;
    size_t m_verticesCount = 0;

    // TODO: use vao
    unsigned int m_vao = 0;

    void recreateHardwareBuffer();
    void updateHardwareBuffer();

public:
    VertexBuffer(const VertexFormat &vf);
    ~VertexBuffer();

    void bind();
    void unbind();

    void resize(size_t vertexCount);
    void clear();
    void appendVertexData(const float *source, size_t vertexCount);

    const float *getVertexData() const { return m_vertexData.data(); }
    float *getVertexData() { return m_vertexData.data(); }
    size_t getVerticesCount() const { return m_verticesCount; }

    size_t getElementsUsed() const;
    const VertexFormat &getFormat() const { return m_vertexFormat; }
};

// Gpu index buffer representation.
class IndexBuffer : public GpuBuffer
{
    IndexArray m_indices;

    void recreateHardwareBuffer();
    void updateHardwareBuffer();

public:
    IndexBuffer(/*TODO: index buffer type*/);
    ~IndexBuffer();

    void appendIndices(const IndexArray &indices);

    void bind();
    void unbind();

    const IndexArray &getIndices() const { return m_indices; }
    IndexArray &getIndices() { return m_indices; }

    INDICES_TYPE *getRawIndices() { if (m_indices.size() == 0) return nullptr; return &m_indices[0]; }

    size_t getElementsUsed() const;
};

// XXX:
// Valid only for quads with vertex format Vertex_3p2tx!!!
// FIXME:
shared_ptr<VertexBuffer> createQuad(const VertexFormat &vf, float x, float y, float w, float h);

// FIXME: keeping this ugly names as reminder to refactor this shit!
// XXX: Valid only for vertex format Vertex_3p2tx4c!!!
shared_ptr<VertexBuffer> createQuad2(const VertexFormat &vf, float x, float y, float w, float h);

} }

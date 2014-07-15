#pragma once

#include "Vertex.h"

namespace df3d { namespace render {

class Material;

// FIXME:
// Make setting.
#define INDICES_16_BIT uint16_t
#define INDICES_32_BIT uint32_t

typedef INDICES_32_BIT INDICES_TYPE;

enum GpuBufferUsageType
{
    GB_USAGE_STATIC,
    GB_USAGE_DYNAMIC,
    GB_USAGE_STREAM
};

typedef std::vector<INDICES_TYPE> IndexArray;

class GpuBuffer : boost::noncopyable
{
protected:
    GpuBufferUsageType m_usageType = GB_USAGE_STATIC;

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

} }
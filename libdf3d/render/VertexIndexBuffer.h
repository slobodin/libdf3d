#pragma once

#include "Vertex.h"
#include "RenderCommon.h"

namespace df3d { namespace render {

//! GPU vertex buffer representation.
class DF3D_DLL VertexBuffer : utils::NonCopyable
{
    // TODO: use vao
    unsigned int m_vao = 0;
    unsigned int m_glId = 0;

    VertexFormat m_format;
    size_t m_verticesUsed = 0;
    size_t m_sizeInBytes = 0;

public:
    VertexBuffer(const VertexFormat &format);
    ~VertexBuffer();

    /*! 
     * \brief Creates new data store for the vertex buffer.
     * \param verticesCount count of vertices, actual size in bytes is computed using vertex format.
     * \param data raw vertex data, usually floats array. May be null.
     * \param usage GPU buffer usage hint.
     * \return
     */
    void alloc(size_t verticesCount, const void *data, GpuBufferUsageType usage);
    //! Creates new data store for a given VertexData.
    void alloc(const VertexData &data, GpuBufferUsageType usage);
    //! Update buffer with new data, verticesCount may be less than initially allocated.
    void update(size_t verticesCount, const void *data);

    void bind();
    void unbind();

    void setVerticesUsed(size_t used) { m_verticesUsed = used; }
    size_t getVerticesUsed() const { return m_verticesUsed; }

    // TODO:
    // Buffer lock/unlock via glMapBuffer
};

//! GPU index buffer representation.
// NOTE: Implementation is mostly similar to the vertex buffer 
// but it may be useful to distinguish these buffers in future.
class DF3D_DLL IndexBuffer : utils::NonCopyable
{
    unsigned int m_glId = 0;
    size_t m_indicesUsed = 0;
    size_t m_sizeInBytes = 0;

public:
    IndexBuffer(/*TODO: index buffer type*/);
    ~IndexBuffer();

    /*! 
     * \brief Creates new data store for the index buffer.
     * \param indicesCount count of vertices, actual size in bytes is computed using index size (hardcoded for now).
     * \param data raw index data,may be null.
     * \param usage GPU buffer usage hint.
     * \return
     */
    void alloc(size_t indicesCount, const void *data, GpuBufferUsageType usage);
    //! Update buffer with new data, indicesCount may be less than initially allocated.
    void update(size_t indicesCount, const void *data);

    void bind();
    void unbind();

    void setIndicesUsed(size_t used) { m_indicesUsed = used; }
    size_t getIndicesUsed() const { return m_indicesUsed; }
};

// XXX:
// Valid only for quads with vertex format Vertex_3p2tx!!!
// FIXME:
shared_ptr<VertexBuffer> createQuad(const VertexFormat &vf, float x, float y, float w, float h, GpuBufferUsageType usage);

// FIXME: keeping this ugly names as reminder to refactor this shit!
// XXX: Valid only for vertex format Vertex_3p2tx4c!!!
shared_ptr<VertexBuffer> createQuad2(const VertexFormat &vf, float x, float y, float w, float h, GpuBufferUsageType usage);

} }

#pragma once

namespace df3d {

// Silly assert but we support only floats.
static_assert(std::is_same<glm::vec3::value_type, float>::value, "glm: only floats are supported");

class DF3D_DLL VertexFormat
{
public:
    enum VertexAttribute
    {
        // FIXME: figure out why if its starting not from 0 gives black screen on mac os.
        // stackoverflow.com/questions/11497870
        // FIXME: reordering may affect on ParticleSystemBuffers_Quad
        POSITION_3,
        TX_2,
        COLOR_4,
        NORMAL_3,
        TANGENT_3,
        BITANGENT_3,

        COUNT
    };

    // FIXME: encapsulate
    std::vector<VertexAttribute> m_attribs;
    size_t m_offsets[COUNT] = { 0 };
    size_t m_counts[COUNT] = { 0 };
    size_t m_size = 0;

    VertexFormat(const std::vector<VertexAttribute> &attribs);

    //! Whether or not this format has a given attribute.
    bool hasAttribute(VertexAttribute attrib) const;
    //! Returns size in bytes.
    size_t getVertexSize() const;
    //! Returns offset in bytes to a given attribute.
    size_t getOffsetTo(VertexAttribute attrib) const;
    //! Return the size in bytes of a given attribute.
    size_t getAttributeSize(VertexAttribute attrib) const;
};

class DF3D_DLL Vertex
{
    float *m_vertexData;
    const VertexFormat &m_format;

    template<typename T>
    T* getPointer(VertexFormat::VertexAttribute attrib)
    {
        return reinterpret_cast<T*>(m_vertexData + m_format.getOffsetTo(attrib) / sizeof(float));
    }

public:
    Vertex(const VertexFormat &format, float *vertexData);
    Vertex(const Vertex &other);

    void setPosition(const glm::vec3 &pos);
    void setTx(const glm::vec2 &tx);
    void setColor(const glm::vec4 &color);
    void setNormal(const glm::vec3 &normal);
    void setTangent(const glm::vec3 &tangent);
    void setBitangent(const glm::vec3 &bitangent);

    void getPosition(glm::vec3 *pos);
    void getTx(glm::vec2 *tx);
    void getColor(glm::vec4 *color);
    void getNormal(glm::vec3 *normal);
    void getTangent(glm::vec3 *tangent);
    void getBitangent(glm::vec3 *bitangent);
};

class DF3D_DLL VertexData
{
    std::vector<float> m_data;
    VertexFormat m_format;
    size_t m_verticesCount = 0;

public:
    VertexData(const VertexFormat &format);
    VertexData(const VertexFormat &format, std::vector<float> &&data);

    //! Allocates memory for given number of vertices.
    void allocVertices(size_t verticesCount);
    //! Allocates memory for a new vertex and returns vertex proxy.
    Vertex allocVertex();
    //! Returns vertex proxy for ith vertex [0, n).
    Vertex getVertex(size_t idx);
    //! Returns this data vertex format.
    const VertexFormat& getFormat() const { return m_format; }
    //! Returns count of vertices in this buffer.
    size_t getVerticesCount() const { return m_verticesCount; }
    //! Returns raw vertex data.
    const float* getRawData() const { return m_data.data(); }
    //! Returns vertex data size in bytes.
    size_t getSizeInBytes() const { return m_data.size() * sizeof(float); }
    //! Clears the buffer.
    void clear();
};

}

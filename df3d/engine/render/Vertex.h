#pragma once

namespace df3d {

// Silly assert but we support only floats.
static_assert(std::is_same<glm::vec3::value_type, float>::value, "glm: only floats are supported");

class DF3D_DLL VertexFormat
{
public:
    enum VertexAttribute : uint16_t
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

private:
    uint16_t m_attribs[COUNT];
    uint16_t m_size;

public:
    VertexFormat();
    VertexFormat(const std::vector<VertexAttribute> &attribs);

    //! Whether or not this format has a given attribute.
    bool hasAttribute(VertexAttribute attrib) const
    {
        return m_attribs[attrib] != 0xFFFF;
    }

    //! Returns vertex size of this format in bytes.
    size_t getVertexSize() const
    {
        return m_size;
    }

    //! Returns offset in bytes to a given attribute.
    uint16_t getOffsetTo(VertexAttribute attrib) const
    {
        return m_attribs[attrib] >> 8;
    }

    //! Returns number of components per vertex attribute. Must be 1, 2, 3 or 4.
    uint16_t getCompCount(VertexAttribute attrib) const
    {
        return m_attribs[attrib] & 0x00FF;
    }
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
    PodArray<float> m_data;
    VertexFormat m_format;
    size_t m_verticesCount = 0;

public:
    VertexData(const VertexFormat &format);
    VertexData(const VertexFormat &format, PodArray<float> &&data);

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

namespace vertex_formats
{
    extern const DF3D_DLL VertexFormat p3_tx2_c4;
    extern const DF3D_DLL VertexFormat p3_n3_tx2_tan3_bitan3;
}

}

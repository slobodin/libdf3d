#pragma once

namespace df3d { namespace render {

// Silly assert but we support only floats.
static_assert(std::is_same<glm::vec3::value_type, float>::value, "glm: only floats are supported");

class VertexFormat
{
public:
    enum VertexAttribute
    {
        POSITION_2,
        POSITION_3,
        TX_2,
        COLOR_4,
        NORMAL_3,
        TANGENT_3,
        BITANGENT_3,

        COUNT
    };

private:
    std::vector<VertexAttribute> m_attribs;
    size_t m_offsets[COUNT] = { 0 };
    size_t m_counts[COUNT] = { 0 };
    size_t m_size = 0;

public:
    VertexFormat(const std::vector<VertexAttribute> &attribs);

    //! Whether or not this format has a given attribute.
    bool hasAttribute(VertexAttribute attrib) const;
    //! Returns size in bytes.
    size_t getVertexSize() const;
    //! Returns offset in bytes to a given attribute.
    size_t getOffsetTo(VertexAttribute attrib) const;
    //! Return the size in bytes of a given attribute.
    size_t getAttributeSize(VertexAttribute attrib) const;

    void enableGLAttributes();
    void disableGLAttributes();

    bool operator== (const VertexFormat &other) const;
    bool operator!= (const VertexFormat &other) const;
};

class Vertex
{
    float *m_vertexData;
    const VertexFormat &m_format;

public:
    Vertex(const VertexFormat &format, float *vertexData);

    void setPosition(const glm::vec2 &pos);
    void setPosition(const glm::vec3 &pos);
    void setTx(const glm::vec2 &tx);
    void setColor(const glm::vec4 &color);
    void setNormal(const glm::vec3 &normal);
    void setTangent(const glm::vec3 &tangent);
    void setBitangent(const glm::vec3 &bitangent);

    void getPosition(glm::vec2 *pos);
    void getPosition(glm::vec3 *pos);
    void getTx(glm::vec2 *tx);
    void getColor(glm::vec4 *color);
    void getNormal(glm::vec3 *normal);
    void getTangent(glm::vec3 *tangent);
    void getBitangent(glm::vec3 *bitangent);
};

class VertexData
{
    std::vector<float> m_data;
    VertexFormat m_format;
    size_t m_verticesCount = 0;

public:
    VertexData(const VertexFormat &format);

    //! Allocates memory for given number of vertices.
    void alloc(size_t verticesCount);
    //! Allocates memory for a new vertex and returns vertex proxy.
    Vertex getNextVertex();
    //! Returns vertex proxy for ith vertex [0, n).
    Vertex getVertex(size_t idx);
    //! Returns this data vertex format.
    const VertexFormat& getFormat() const { return m_format; }
    //! Returns count of vertices in this buffer.
    size_t getVerticesCount() const { return m_verticesCount; }
    //! Returns raw vertex data.
    const float* getRawData() const { return m_data.data(); }
    //! Returns raw vertex data.
    float* getRawData() { return m_data.data(); }
    //! Clears the buffer.
    void clear();
};

} }

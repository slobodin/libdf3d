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
        POSITION,       // glm::vec3
        TX,             // glm::vec2
        COLOR,          // glm::vec4
        NORMAL,         // glm::vec3
        TANGENT,        // glm::vec3
        BITANGENT,      // glm::vec3

        COUNT
    };

private:
    uint16_t m_attribs[COUNT];
    uint16_t m_size;

public:
    VertexFormat();
    VertexFormat(std::initializer_list<VertexAttribute> attribs);

    //! Whether or not this format has a given attribute.
    bool hasAttribute(VertexAttribute attrib) const { return m_attribs[attrib] != 0xFFFF; }

    //! Returns vertex size of this format in bytes.
    size_t getVertexSize() const { return m_size; }

    //! Returns offset in bytes to a given attribute.
    uint16_t getOffsetTo(VertexAttribute attrib) const { return m_attribs[attrib] >> 8; }

    //! Returns number of components per vertex attribute. Must be 1, 2, 3 or 4.
    uint16_t getCompCount(VertexAttribute attrib) const { return m_attribs[attrib] & 0x00FF; }

    bool operator== (const VertexFormat &other) const
    {
        if (m_size != other.m_size)
            return false;

        for (uint16_t i = 0; i < COUNT; i++)
        {
            if (m_attribs[i] != other.m_attribs[i])
                return false;
        }

        return true;
    }

    bool operator!= (const VertexFormat &other) const
    {
        return !(*this == other);
    }
};

class DF3D_DLL VertexData
{
    PodArray<uint8_t> m_data;
    VertexFormat m_format;

public:
    VertexData(const VertexFormat &format);

    void addVertices(size_t verticesCount);
    void addVertex();

    void* getVertex(size_t idx);
    void* getVertexAttribute(size_t idx, VertexFormat::VertexAttribute attrib);
    const VertexFormat& getFormat() const { return m_format; }
    size_t getVerticesCount() const;
    void* getRawData() { return m_data.data(); }
    const void* getRawData() const { return m_data.data(); }
    size_t getSizeInBytes() const { return m_data.size(); }
};

struct DF3D_DLL Vertex_p_c
{
    glm::vec3 pos;
    glm::vec4 color;

    static const VertexFormat& getFormat();
};

struct DF3D_DLL Vertex_p_tx_c
{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec4 color;

    static const VertexFormat& getFormat();
};

struct DF3D_DLL Vertex_p_n_tx_tan_bitan
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    static const VertexFormat& getFormat();
};

}

#pragma once

namespace df3d { namespace render {

//enum VertexFormat
//{
//    VF_INVALID
//};

// TODO:
// Align 4 bytes?

#pragma pack(push, 1)

//! Vertex of a mesh.
//struct Vertex
//{
//    //! Vertex point.
//    glm::vec3 p;
//    //! Vertex normal.
//    glm::vec3 n;
//    //! Vertex texture coordinates.
//    glm::vec2 t;
//    //! Color.
//    glm::vec4 color;
//    //! Tangent space basis.
//    glm::vec3 tangent;
//    glm::vec3 bitangent;
//
//    Vertex()
//    {
//        color.r = color.g = color.b = color.a = 1.0f;
//    }
//
//    static unsigned int ptOffset() { return 0; }
//    static unsigned int normalsOffset() { return sizeof(glm::vec3); }
//    static unsigned int txCoordOffset() { return sizeof(glm::vec3) + sizeof(glm::vec3); }
//    static unsigned int colorOffset() { return txCoordOffset() + sizeof(glm::vec2); }
//    static unsigned int tangentOffset() { return colorOffset() + sizeof(glm::vec4); }
//    static unsigned int bitangentOffset() { return tangentOffset() + sizeof(glm::vec3); }
//
//    bool operator< (const Vertex &other) const
//    {
//        return memcmp(this, &other, sizeof(Vertex)) > 0;
//    }
//};

#pragma pack(pop)

class VertexFormat;

class VertexComponent
{
    friend class VertexFormat;
public:
    enum Type
    {
        POSITION = 0,
        NORMAL,
        TEXTURE_COORDS,
        COLOR,
        TANGENT,
        BITANGENT
    };

private:
    Type m_type = POSITION;
    size_t m_count = 0;

    VertexComponent(Type type, size_t count)
        : m_type(type), m_count(count)
    {

    }

public:
    Type getType() const { return m_type; }
    size_t getCount() const { return m_count; }
};

class VertexFormat
{
    std::vector<VertexComponent> m_components;
    size_t m_vertexSize = 0;    // bytes

    VertexFormat() { }

    void addComponent(const VertexComponent &component);

public:
    //! Returns size in bytes.
    size_t getVertexSize() const { return m_vertexSize; }
    void enableGLAttributes() const;
    void disableGLAttributes() const;

    //! Returns offset in bytes to the component of provided type.
    size_t getOffsetTo(VertexComponent::Type component) const;

    bool hasComponent(VertexComponent::Type component) const;
    const VertexComponent *getComponent(VertexComponent::Type component) const;

    static VertexFormat create(const std::string &definition);
};

#pragma pack(push, 1)

struct Vertex_3p3n2tx4c3t3b
{
    glm::vec3 p;
    glm::vec3 n;
    glm::vec2 tx;
    glm::vec4 color;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    Vertex_3p3n2tx4c3t3b()
    {
        color.r = color.g = color.b = color.a = 1.0f;
    }
};

struct Vertex_3p3n2tx3t3b
{
    glm::vec3 p;
    glm::vec3 n;
    glm::vec2 tx;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct Vertex_3p2tx
{
    glm::vec3 p;
    glm::vec2 tx;
};

struct Vertex_3p2tx4c
{
    glm::vec3 p;
    glm::vec2 tx;
    glm::vec4 color;

    Vertex_3p2tx4c()
    {
        color.r = color.g = color.b = color.a = 1.0f;
    }
};

struct Vertex_2p2tx4c
{
    glm::vec2 p;
    glm::vec2 tx;
    glm::vec4 color;

    Vertex_2p2tx4c()
    {
        color.r = color.g = color.b = color.a = 1.0f;
    }
};

#pragma pack(pop)

} }
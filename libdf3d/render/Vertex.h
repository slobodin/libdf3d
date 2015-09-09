#pragma once

namespace df3d { namespace render {

class VertexFormat;

class DF3D_DLL VertexComponent
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

class DF3D_DLL VertexFormat
{
    std::vector<VertexComponent> m_components;
    size_t m_vertexSize = 0;    // bytes

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

// Silly assert but we support only floats.
static_assert(std::is_same<glm::vec3::value_type, float>::value, "glm: only floats are supported");

#pragma pack(push, 1)

struct DF3D_DLL Vertex_3p3n2tx4c3t3b
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

struct DF3D_DLL Vertex_3p3n2tx3t3b
{
    glm::vec3 p;
    glm::vec3 n;
    glm::vec2 tx;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct DF3D_DLL Vertex_3p2tx
{
    glm::vec3 p;
    glm::vec2 tx;
};

struct DF3D_DLL Vertex_3p2tx4c
{
    glm::vec3 p;
    glm::vec2 tx;
    glm::vec4 color;

    Vertex_3p2tx4c()
    {
        color.r = color.g = color.b = color.a = 1.0f;
    }
};

struct DF3D_DLL Vertex_2p2tx4c
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

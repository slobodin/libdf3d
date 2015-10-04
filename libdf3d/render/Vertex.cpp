#include "Vertex.h"

#include "OpenGLCommon.h"
#include <utils/Utils.h>

namespace df3d { namespace render {

VertexFormat::VertexFormat(const std::vector<VertexAttribute> &attribs)
    : m_attribs(attribs)
{
    size_t offset = 0;
    for (auto attrib : m_attribs)
    {
        m_size += getAttributeSize(attrib);
        m_offsets[attrib] = offset;
        offset = m_size;

        switch (attrib)
        {
        case VertexFormat::TX_2:
            m_counts[attrib] = 2;
            break;
        case VertexFormat::POSITION_3:
        case VertexFormat::NORMAL_3:
        case VertexFormat::TANGENT_3:
        case VertexFormat::BITANGENT_3:
            m_counts[attrib] = 3;
            break;
        case VertexFormat::COLOR_4:
            m_counts[attrib] = 4;
            break;
        default:
            assert(false);
        }
    }
}

bool VertexFormat::hasAttribute(VertexAttribute attrib) const
{
    return utils::contains(m_attribs, attrib);
}

size_t VertexFormat::getVertexSize() const
{
    return m_size;
}

size_t VertexFormat::getOffsetTo(VertexAttribute attrib) const
{
    return m_offsets[attrib];
}

size_t VertexFormat::getAttributeSize(VertexAttribute attrib) const
{
    switch (attrib)
    {
    case VertexFormat::POSITION_3:
        return 3 * sizeof(float);
    case VertexFormat::TX_2:
        return 2 * sizeof(float);
    case VertexFormat::COLOR_4:
        return 4 * sizeof(float);
    case VertexFormat::NORMAL_3:
        return 3 * sizeof(float);
    case VertexFormat::TANGENT_3:
        return 3 * sizeof(float);
    case VertexFormat::BITANGENT_3:
        return 3 * sizeof(float);
    default:
        break;
    }

    assert(false && "no such attribute in vertex format");
    return 0;
}

void VertexFormat::enableGLAttributes()
{
    for (auto attrib : m_attribs)
    {
        glEnableVertexAttribArray(attrib);
        size_t offs = getOffsetTo(attrib);
        glVertexAttribPointer(attrib, m_counts[attrib], GL_FLOAT, GL_FALSE, getVertexSize(), (const GLvoid*)offs);
    }
}

void VertexFormat::disableGLAttributes()
{
    for (auto attrib : m_attribs)
        glDisableVertexAttribArray(attrib);
}

bool VertexFormat::operator== (const VertexFormat &other) const
{
    if (m_size != other.m_size)
        return false;

    if (m_attribs.size() != other.m_attribs.size())
        return 0;

    for (size_t i = 0; i < m_attribs.size(); i++)
    {
        if (m_attribs[i] != other.m_attribs[i])
            return false;
    }

    return true;
}

bool VertexFormat::operator!= (const VertexFormat &other) const
{
    return !(*this == other);
}

Vertex::Vertex(const VertexFormat &format, float *vertexData)
    : m_format(format)
{
    m_vertexData = vertexData;
}

Vertex::Vertex(const Vertex &other)
    : m_format(other.m_format),
    m_vertexData(other.m_vertexData)
{

}

void Vertex::setPosition(const glm::vec3 &pos)
{
    *getPointer<glm::vec3>(VertexFormat::POSITION_3) = pos;
}

void Vertex::setTx(const glm::vec2 &tx)
{
    *getPointer<glm::vec2>(VertexFormat::TX_2) = tx;
}

void Vertex::setColor(const glm::vec4 &color)
{
    *getPointer<glm::vec4>(VertexFormat::COLOR_4) = color;
}

void Vertex::setNormal(const glm::vec3 &normal)
{
    *getPointer<glm::vec3>(VertexFormat::NORMAL_3) = normal;
}

void Vertex::setTangent(const glm::vec3 &tangent)
{
    *getPointer<glm::vec3>(VertexFormat::TANGENT_3) = tangent;
}

void Vertex::setBitangent(const glm::vec3 &bitangent)
{
    *getPointer<glm::vec3>(VertexFormat::BITANGENT_3) = bitangent;
}

void Vertex::getPosition(glm::vec3 *pos)
{
    *pos = *getPointer<glm::vec3>(VertexFormat::POSITION_3);
}

void Vertex::getTx(glm::vec2 *tx)
{
    *tx = *getPointer<glm::vec2>(VertexFormat::TX_2);
}

void Vertex::getColor(glm::vec4 *color)
{
    *color = *getPointer<glm::vec4>(VertexFormat::COLOR_4);
}

void Vertex::getNormal(glm::vec3 *normal)
{
    *normal = *getPointer<glm::vec3>(VertexFormat::NORMAL_3);
}

void Vertex::getTangent(glm::vec3 *tangent)
{
    *tangent = *getPointer<glm::vec3>(VertexFormat::TANGENT_3);
}

void Vertex::getBitangent(glm::vec3 *bitangent)
{
    *bitangent = *getPointer<glm::vec3>(VertexFormat::BITANGENT_3);
}

VertexData::VertexData(const VertexFormat &format)
    : m_format(format)
{

}

void VertexData::allocVertices(size_t verticesCount)
{
    assert(verticesCount > 0);

    m_data.assign(m_format.getVertexSize() * verticesCount / sizeof(float), 0.0f);

    m_verticesCount = verticesCount;
}

Vertex VertexData::allocVertex()
{
    // Allocate buffer for a new vertex.
    auto it = m_data.insert(m_data.end(), m_format.getVertexSize() / sizeof(float), 0.0f);
    // Get pointer to this vertex raw data.
    auto vertexData = m_data.data() + (it - m_data.begin());

    m_verticesCount++;

    // Return vertex proxy.
    return Vertex(m_format, vertexData);
}

Vertex VertexData::getVertex(size_t idx)
{
    assert(idx < m_verticesCount);

    return Vertex(m_format, m_data.data() + m_format.getVertexSize() * idx / sizeof(float));
}

void VertexData::clear()
{
    m_verticesCount = 0;
    m_data.clear();
}

} }

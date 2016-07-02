#include "Vertex.h"

#include <df3d/lib/Utils.h>
#include <df3d/engine/EngineController.h>

namespace df3d {

static uint16_t GetAttributeSize(VertexFormat::VertexAttribute attrib)
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

    DF3D_ASSERT_MESS(false, "no such attribute in vertex format");
    return 0;
}

static uint16_t GetAttributeCompCount(VertexFormat::VertexAttribute attrib)
{
    switch (attrib)
    {
    case VertexFormat::TX_2:
        return 2;
    case VertexFormat::POSITION_3:
    case VertexFormat::NORMAL_3:
    case VertexFormat::TANGENT_3:
    case VertexFormat::BITANGENT_3:
        return 3;
    case VertexFormat::COLOR_4:
        return 4;
    default:
        DF3D_ASSERT(false);
    }

    return 0;
}

VertexFormat::VertexFormat()
    : m_size(0)
{
    memset(&m_attribs, 0xFFFF, sizeof(m_attribs));
}

VertexFormat::VertexFormat(const std::vector<VertexAttribute> &attribs)
    : VertexFormat()
{
    uint16_t totalOffset = 0;
    for (auto attrib : attribs)
    {
        uint16_t attribSize = GetAttributeSize(attrib);
        uint16_t attribCompCount = GetAttributeCompCount(attrib);
        DF3D_ASSERT(attribCompCount >= 1 && attribCompCount <= 4);

        m_attribs[attrib] = (totalOffset << 8) | attribCompCount;

        m_size += attribSize;
        totalOffset = m_size;
        DF3D_ASSERT(totalOffset <= (2 << 8));
    }
}

Vertex::Vertex(const VertexFormat &format, float *vertexData)
    : m_format(format)
{
    m_vertexData = vertexData;
}

Vertex::Vertex(const Vertex &other)
    : m_vertexData(other.m_vertexData),
    m_format(other.m_format)
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
    : m_data(MemoryManager::allocDefault()),
    m_format(format)
{

}

VertexData::VertexData(const VertexFormat &format, PodArray<float> &&data)
    : m_data(std::move(data)),
    m_format(format)
{
    m_verticesCount = (m_data.size() * sizeof(float)) / m_format.getVertexSize();
}

void VertexData::allocVertices(size_t verticesCount)
{
    DF3D_ASSERT(verticesCount > 0);

    size_t floatsCount = m_format.getVertexSize() * verticesCount / sizeof(float);
    m_data.resize(floatsCount);

    m_verticesCount = verticesCount;
}

Vertex VertexData::allocVertex()
{
    // Allocate buffer for a new vertex.
    size_t floatsCount = m_format.getVertexSize() / sizeof(float);
    m_data.resize(m_data.size() + floatsCount);

    // Get pointer to this vertex raw data.
    auto vertexData = m_data.end() - floatsCount;

    m_verticesCount++;

    // Return vertex proxy.
    return Vertex(m_format, vertexData);
}

Vertex VertexData::getVertex(size_t idx)
{
    DF3D_ASSERT(idx < m_verticesCount);

    return Vertex(m_format, m_data.data() + m_format.getVertexSize() * idx / sizeof(float));
}

void VertexData::clear()
{
    m_verticesCount = 0;
    m_data.clear();
}

namespace vertex_formats
{

const VertexFormat p3_tx2_c4 = VertexFormat({ VertexFormat::POSITION_3, VertexFormat::TX_2, VertexFormat::COLOR_4 });
const VertexFormat p3_n3_tx2_tan3_bitan3 = VertexFormat({ VertexFormat::POSITION_3, VertexFormat::NORMAL_3,
                                                        VertexFormat::TX_2, VertexFormat::TANGENT_3, VertexFormat::BITANGENT_3 });

}

}

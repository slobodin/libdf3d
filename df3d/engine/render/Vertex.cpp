#include "Vertex.h"

#include <df3d/lib/Utils.h>
#include <df3d/engine/EngineController.h>

namespace df3d {

static uint16_t GetAttributeSize(VertexFormat::VertexAttribute attrib)
{
    switch (attrib)
    {
    case VertexFormat::POSITION:
        return 3 * sizeof(float);
    case VertexFormat::TX:
        return 2 * sizeof(float);
    case VertexFormat::COLOR:
        return 4 * sizeof(float);
    case VertexFormat::NORMAL:
        return 3 * sizeof(float);
    case VertexFormat::TANGENT:
        return 3 * sizeof(float);
    case VertexFormat::BITANGENT:
        return 3 * sizeof(float);
    default:
        DF3D_ASSERT_MESS(false, "no such attribute in vertex format");
    }

    return 0;
}

static uint16_t GetAttributeCompCount(VertexFormat::VertexAttribute attrib)
{
    switch (attrib)
    {
    case VertexFormat::TX:
        return 2;
    case VertexFormat::POSITION:
    case VertexFormat::NORMAL:
    case VertexFormat::TANGENT:
    case VertexFormat::BITANGENT:
        return 3;
    case VertexFormat::COLOR:
        return 4;
    default:
        DF3D_ASSERT_MESS(false, "Unknown vertex attribute");
    }

    return 0;
}

VertexFormat::VertexFormat()
    : m_size(0)
{
    memset(&m_attribs, 0xFFFF, sizeof(m_attribs));
}

VertexFormat::VertexFormat(std::initializer_list<VertexAttribute> attribs)
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

VertexData::VertexData(const VertexFormat &format)
    : m_data(MemoryManager::allocDefault()),
    m_format(format)
{

}

void VertexData::addVertices(size_t verticesCount)
{
    DF3D_ASSERT(verticesCount > 0);

    m_data.resize(m_format.getVertexSize() * verticesCount + m_data.size());
}

void VertexData::addVertex()
{
    m_data.resize(m_data.size() + m_format.getVertexSize());
}

void* VertexData::getVertex(size_t idx)
{
    DF3D_ASSERT(idx < getVerticesCount());

    return m_data.data() + m_format.getVertexSize() * idx;
}

void* VertexData::getVertexAttribute(size_t idx, VertexFormat::VertexAttribute attrib)
{
    DF3D_ASSERT(m_format.hasAttribute(attrib));

    auto vertex = (uint8_t*)getVertex(idx);
    return vertex + m_format.getOffsetTo(attrib);
}

size_t VertexData::getVerticesCount() const
{
    return m_data.size() / m_format.getVertexSize();
}

const VertexFormat& Vertex_p_c::getFormat()
{
    static VertexFormat format = { VertexFormat::POSITION, VertexFormat::COLOR };
    return format;
}

const VertexFormat& Vertex_p_tx_c::getFormat()
{
    static VertexFormat format = { VertexFormat::POSITION, VertexFormat::TX, VertexFormat::COLOR };
    return format;
}

const VertexFormat& Vertex_p_n_tx_tan_bitan::getFormat()
{
    static VertexFormat format = { VertexFormat::POSITION, VertexFormat::NORMAL,
        VertexFormat::TX, VertexFormat::TANGENT, VertexFormat::BITANGENT };
    return format;
}

}

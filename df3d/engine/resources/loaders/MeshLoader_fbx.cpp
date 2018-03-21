#include "MeshLoader_fbx.h"

#include <df3d/engine/render/MeshUtils.h>
#include <df3d/engine/EngineCVars.h>
#include <df3d/engine/resources/MeshResource.h>
#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/render/Vertex.h>
#include <df3d/lib/Utils.h>
#include <zlib.h>

namespace df3d {

namespace {

std::string ReadString(ResourceDataSource &dataSource)
{
    uint32_t len;
    dataSource.read(&len, 4);

    std::string res;
    for (uint32_t i = 0; i < len; i++)
    {
        char ch;
        dataSource.read(&ch, 1);
        res.push_back(ch);
    }
    return res;
}

void Skip(ResourceDataSource &dataSource, int bytes)
{
    dataSource.seek(bytes, SeekDir::CURRENT);
}

class FBXProperty
{
    struct Val
    {
        union Primitive
        {
            int16_t i16;
            int32_t i32;
            int8_t boolean;
            float f;
            double d;
            int64_t i64;
        } primitive;
        std::string string;
        std::vector<double> arrDouble;
        std::vector<float> arrFloat;
        std::vector<int64_t> arrLong;
        std::vector<int32_t> arrInt;
    };

    Val m_value;
    uint8_t m_typeCode = 0;

    static_assert(sizeof(double) == 8);
    static_assert(sizeof(float) == 4);

    template<typename T>
    static std::vector<T> Convert(const std::vector<uint8_t> &rawBuffer)
    {
        DF3D_ASSERT(rawBuffer.size() % sizeof(T) == 0);

        std::vector<T> result;
        for (size_t i = 0; i < rawBuffer.size(); i += sizeof(T))
        {
            union
            {
                uint8_t bytes[sizeof(T)];
                T value;
            } value;

            memcpy(&value, &rawBuffer[i], sizeof(T));
            result.push_back(value.value);
        }

        return result;
    }

    void fillArray(const std::vector<uint8_t> &rawBuffer)
    {
        if (m_typeCode == 'i')
            m_value.arrInt = Convert<int32_t>(rawBuffer);
        else if (m_typeCode == 'f')
            m_value.arrFloat = Convert<float>(rawBuffer);
        else if (m_typeCode == 'd')
            m_value.arrDouble = Convert<double>(rawBuffer);
        else if (m_typeCode == 'l')
            m_value.arrLong = Convert<int64_t>(rawBuffer);
    }

public:
    void readFromFile(ResourceDataSource &dataSource)
    {
        dataSource.read(&m_typeCode, 1);
        int itemSize = 0;
        bool isArray = false;

        switch (m_typeCode)
        {
            case 'Y':
                dataSource.read(&m_value.primitive.i16, 2);
                break;
            case 'C':
                dataSource.read(&m_value.primitive.boolean, 1);
                break;
            case 'I':
                dataSource.read(&m_value.primitive.i32, 4);
                break;
            case 'F':
                dataSource.read(&m_value.primitive.f, 4);
                break;
            case 'D':
                dataSource.read(&m_value.primitive.d, 8);
                break;
            case 'L':
                dataSource.read(&m_value.primitive.i64, 8);
                break;
            case 'R':
            {
                uint32_t len;
                dataSource.read(&len, 4);

                Skip(dataSource, len);
            }
            break;
            case 'S':
                m_value.string = ReadString(dataSource);
                break;

            case 'i':
            case 'f':
                itemSize = 4;
                isArray = true;
                break;
            case 'd':
            case 'l':
                itemSize = 8;
                isArray = true;
                break;
            default:
                DF3D_ASSERT(false);
        }

        if (isArray)
        {
            uint32_t arrayLen, encoding, compressedLen;

            dataSource.read(&arrayLen, 4);
            dataSource.read(&encoding, 4);
            dataSource.read(&compressedLen, 4);

            std::vector<uint8_t> dataBuffer(compressedLen);
            dataSource.read(dataBuffer.data(), compressedLen);

            // Raw data.
            if (encoding == 0)
            {
                DF3D_ASSERT(itemSize * arrayLen == compressedLen);
            }
            // Deflated.
            else if (encoding == 1)
            {
                auto uncompressedLen = itemSize * arrayLen;

                uLong destLen = uncompressedLen;
                std::vector<uint8_t> decompressedBuffer(destLen);

                auto uncompressRes = uncompress(decompressedBuffer.data(), &destLen, dataBuffer.data(), compressedLen);

                DF3D_ASSERT(uncompressRes == Z_OK);
                DF3D_ASSERT(destLen == uncompressedLen);

                dataBuffer = std::move(decompressedBuffer);
            }
            else
            {
                DF3D_ASSERT(false);
            }

            fillArray(dataBuffer);
        }
    }

    std::vector<glm::vec3> toVec3Array() const
    {
        DF3D_ASSERT(m_typeCode == 'd');
        DF3D_ASSERT(m_value.arrDouble.size() % 3 == 0);

        std::vector<glm::vec3> result;
        for (int i = 0; i < m_value.arrDouble.size(); i += 3)
        {
            float x = (float)m_value.arrDouble[i + 0];
            float y = (float)m_value.arrDouble[i + 1];
            float z = (float)m_value.arrDouble[i + 2];
            result.push_back({ x, y, z });
        }
        return result;
    }

    PodArray<uint16_t> toUint16ArrayAsIndices(Allocator &alloc) const
    {
        PodArray<uint16_t> result(alloc);
        DF3D_ASSERT(m_typeCode == 'i');

        for (auto v : m_value.arrInt)
        {
            DF3D_ASSERT(v < 0xFFFF);
            if (v < 0)
                v = -v - 1;
            result.push_back(v);
        }

        return result;
    }

    std::string toString() const
    {
        DF3D_ASSERT(m_typeCode == 'S');
        return m_value.string;
    }
};

class FBXNode
{
    std::string m_name;
    std::vector<unique_ptr<FBXNode>> m_children;
    std::vector<FBXProperty> m_properties;

    static unique_ptr<FBXNode> ReadNode(ResourceDataSource &dataSource)
    {
#pragma pack(push, 1)
        struct NodeHeader
        {
            uint32_t endOffset = 0;
            uint32_t numProps = 0;
            uint32_t propertyListLen = 0;
            uint8_t nameLen = 0;
        };
#pragma pack(pop)

        static const NodeHeader NullHeader;

        NodeHeader nodeHeader;
        dataSource.read(&nodeHeader, sizeof(NodeHeader));

        // 13 bytes NULL record reached.
        if (memcmp(&nodeHeader, &NullHeader, sizeof(NodeHeader)) == 0)
            return nullptr;

        auto fbxNode = make_unique<FBXNode>();

        for (uint8_t i = 0; i < nodeHeader.nameLen; i++)
        {
            char ch;
            dataSource.read(&ch, 1);
            fbxNode->m_name.push_back(ch);
        }

        for (uint32_t i = 0; i < nodeHeader.numProps; i++)
        {
            FBXProperty property;
            property.readFromFile(dataSource);
            fbxNode->m_properties.push_back(std::move(property));
        }

        while (dataSource.tell() < nodeHeader.endOffset)
        {
            auto child = ReadNode(dataSource);
            if (child)
                fbxNode->m_children.push_back(std::move(child));
        }

        return fbxNode;
    }

public:
    static unique_ptr<FBXNode> FromFile(ResourceDataSource &dataSource)
    {
#pragma pack(push, 1)
        struct FBXHeader
        {
            uint8_t magic[21];
            uint16_t unknown;
            uint32_t version;
        };
#pragma pack(pop)
        static_assert(sizeof(FBXHeader) == 0x1b);

        FBXHeader header;
        dataSource.read(&header, sizeof(FBXHeader));

        if (strncmp((const char *)header.magic, "Kaydara FBX Binary", 18))
        {
            DFLOG_WARN("Invalid FBX magic");
            return nullptr;
        }

        auto rootFbxNode = make_unique<FBXNode>();

        while (dataSource.tell() < dataSource.getSize())
        {
            auto node = ReadNode(dataSource);
            if (!node)
                break;

            rootFbxNode->m_children.push_back(std::move(node));
        }

        return rootFbxNode;
    }

    const std::string& getName() const { return m_name; }

    std::vector<FBXNode*> getNodes(const std::string &name) const
    {
        std::vector<FBXNode*> result;

        for (const auto &n : m_children)
        {
            if (n->m_name == name)
                result.push_back(n.get());
        }
        return result;
    }

    const FBXProperty* firstProperty() const
    {
        DF3D_ASSERT(m_properties.size() == 1);
        return &m_properties[0];
    }
};

std::vector<glm::vec3> GetVerticesData(ResourceDataSource &dataSource, const FBXNode &node)
{
    auto vertsNode = node.getNodes("Vertices");
    DF3D_ASSERT(vertsNode.size() == 1);
    return vertsNode[0]->firstProperty()->toVec3Array();
}

std::vector<glm::vec3> GetNormalsData(ResourceDataSource &dataSource, const FBXNode &node)
{
    auto normalsNode = node.getNodes("LayerElementNormal");
    DF3D_ASSERT(normalsNode.size() == 1);

#ifdef _DEBUG
    DF3D_ASSERT(normalsNode[0]->getNodes("MappingInformationType").at(0)->firstProperty()->toString() == "ByPolygonVertex");
    DF3D_ASSERT(normalsNode[0]->getNodes("ReferenceInformationType").at(0)->firstProperty()->toString() == "Direct");
#endif

    normalsNode = normalsNode[0]->getNodes("Normals");
    DF3D_ASSERT(normalsNode.size() == 1);

    return normalsNode[0]->firstProperty()->toVec3Array();
}

df3d::PodArray<uint16_t> GetIndexData(ResourceDataSource &dataSource, const FBXNode &node, Allocator &alloc)
{
    auto indicesNode = node.getNodes("PolygonVertexIndex");
    DF3D_ASSERT(indicesNode.size() == 1);

    return indicesNode[0]->firstProperty()->toUint16ArrayAsIndices(alloc);
}

MeshResourceData::Part* ParseGeometry(ResourceDataSource &dataSource, const FBXNode &node, Allocator &alloc)
{
    auto verts = GetVerticesData(dataSource, node);
    auto normals = GetNormalsData(dataSource, node);

    DF3D_ASSERT(!verts.empty() && !normals.empty());

    auto vf = Vertex_p_n_tx_tan_bitan::getFormat();

    auto meshPart = MAKE_NEW(alloc, MeshResourceData::Part)(vf, alloc);
    meshPart->vertexData.addVertices(verts.size());
    meshPart->indicesType = INDICES_16_BIT;

    int i = 0;
    for (auto pos : verts)
    {
        auto v = (Vertex_p_n_tx_tan_bitan*)meshPart->vertexData.getVertex(i);
        v->pos = pos;
        v->tangent = {};
        v->bitangent = {};
        v->normal = {};
        v->uv = {};

        i++;
    }

    meshPart->indexData = GetIndexData(dataSource, node, alloc);

    DF3D_ASSERT(meshPart->indexData.size() > 0 && meshPart->indexData.size() == normals.size());

    const auto vData = (Vertex_p_n_tx_tan_bitan*)meshPart->vertexData.getRawData();
    const auto vCount = meshPart->vertexData.getVerticesCount();
    MeshUtils::computeTangentBasis(vData, vCount);

    meshPart->materialName = "02___Default";

    return meshPart;
}

}

MeshResourceData* MeshLoader_fbx(ResourceDataSource &dataSource, Allocator &alloc)
{
    auto root = FBXNode::FromFile(dataSource);
    if (!root)
        return nullptr;

    auto result = MAKE_NEW(alloc, MeshResourceData)();

    auto objects = root->getNodes("Objects");
    DF3D_ASSERT(objects.size() == 1);

    auto geometries = objects[0]->getNodes("Geometry");
    for (auto geom : geometries)
    {
        auto part = ParseGeometry(dataSource, *geom, alloc);
        if (part)
            result->parts.push_back(part);
    }

    return result;
}

}

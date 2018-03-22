#include "MeshLoader_fbx.h"

#include <df3d/engine/render/MeshUtils.h>
#include <df3d/engine/EngineCVars.h>
#include <df3d/engine/resources/MeshResource.h>
#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/render/Vertex.h>
#include <df3d/lib/Utils.h>
#include <zlib.h>


/*

   PROTOTYPE!!!!!!!!!!!

*/


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

struct FBXModel
{
    glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
    glm::vec3 rotation;
    glm::vec3 translation;
    glm::vec3 geometricTranslation;
    int64_t id = 0;
    std::string name;
};

struct FBXGeometry
{
    int64_t id = 0;
    std::string name;
    MeshResourceData::Part *part = nullptr;
};

struct FBXConnection
{
    int64_t a = -1;
    int64_t b = -1;
    bool isvalid() const { return a != -1 && b != -1; }
};

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

    std::vector<glm::vec2> toVec2Array() const
    {
        DF3D_ASSERT(m_typeCode == 'd');
        DF3D_ASSERT(m_value.arrDouble.size() % 2 == 0);

        std::vector<glm::vec2> result;
        for (int i = 0; i < m_value.arrDouble.size(); i += 2)
        {
            float x = (float)m_value.arrDouble[i + 0];
            float y = (float)m_value.arrDouble[i + 1];
            result.push_back({ x, y });
        }
        return result;
    }

    std::vector<uint16_t> toUint16ArrayAsIndices() const
    {
        std::vector<uint16_t> result;
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

    std::vector<int32_t> toIntArray() const
    {
        std::vector<int32_t> result;
        DF3D_ASSERT(m_typeCode == 'i');

        return m_value.arrInt;
    }

    bool isString() const
    {
        return m_typeCode == 'S';
    }

    std::string toString() const
    {
        DF3D_ASSERT(m_typeCode == 'S');
        return m_value.string;
    }

    double toDouble() const
    {
        DF3D_ASSERT(m_typeCode == 'D');
        return m_value.primitive.d;
    }

    int32_t asInt() const
    {
        DF3D_ASSERT(m_typeCode == 'I');
        return m_value.primitive.i32;
    }

    int64_t asLong() const
    {
        DF3D_ASSERT(m_typeCode == 'L');
        return m_value.primitive.i64;
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
        return &m_properties[0];
    }

    const std::vector<FBXProperty>& properties() const { return m_properties; }
};

std::vector<glm::vec3> GetVerticesData(const FBXNode &node)
{
    auto vertsNode = node.getNodes("Vertices");
    DF3D_ASSERT(vertsNode.size() == 1);
    return vertsNode[0]->firstProperty()->toVec3Array();
}

std::vector<glm::vec3> GetNormalsData(const FBXNode &node)
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

std::vector<uint16_t> GetIndexData(const FBXNode &node)
{
    auto indicesNode = node.getNodes("PolygonVertexIndex");
    DF3D_ASSERT(indicesNode.size() == 1);

    return indicesNode[0]->firstProperty()->toUint16ArrayAsIndices();
}

struct UVData
{
    std::vector<glm::vec2> uv;
    std::vector<int32_t> index;
};

UVData GetUVData(const FBXNode &node)
{
    auto indicesNode = node.getNodes("LayerElementUV");

#ifdef _DEBUG
    DF3D_ASSERT(indicesNode[0]->getNodes("MappingInformationType").at(0)->firstProperty()->toString() == "ByPolygonVertex");
    DF3D_ASSERT(indicesNode[0]->getNodes("ReferenceInformationType").at(0)->firstProperty()->toString() == "IndexToDirect");
#endif

    auto uvProp = indicesNode[0]->getNodes("UV").at(0)->firstProperty();
    auto uvIndexProp = indicesNode[0]->getNodes("UVIndex").at(0)->firstProperty();

    return { uvProp->toVec2Array(), uvIndexProp->toIntArray() };
}

FBXGeometry ParseGeometry(const FBXNode &node, Allocator &alloc)
{
    auto verts = GetVerticesData(node);
    auto normals = GetNormalsData(node);
    auto indices = GetIndexData(node);
    auto uvdata = GetUVData(node);

    DF3D_ASSERT(!verts.empty() && !normals.empty());
    DF3D_ASSERT(indices.size() > 0 && indices.size() == normals.size() && uvdata.index.size() == normals.size());
    DF3D_ASSERT(indices.size() % 3 == 0);

    auto vf = Vertex_p_n_tx_tan_bitan::getFormat();

    auto meshPart = MAKE_NEW(alloc, MeshResourceData::Part)(vf, alloc);
    meshPart->indicesType = INDICES_16_BIT;

    int i = 0;
    for (auto index : indices)
    {
        meshPart->vertexData.addVertex();
        auto v = (Vertex_p_n_tx_tan_bitan*)meshPart->vertexData.getVertex(i);

        auto pos = verts.at(index);
        auto normal = normals.at(i);
        auto uvIndex = uvdata.index.at(i);
        auto uv = uvdata.uv.at(uvIndex);

        v->pos = pos;
        v->tangent = {};
        v->bitangent = {};
        v->normal = normal;
        v->uv = uv;

        i++;
    }

    const auto vData = (Vertex_p_n_tx_tan_bitan*)meshPart->vertexData.getRawData();
    const auto vCount = meshPart->vertexData.getVerticesCount();
    MeshUtils::computeTangentBasis(vData, vCount);

    meshPart->materialName = "02___Default";

    FBXGeometry geom;
    geom.part = meshPart;
    geom.id = node.firstProperty()->asLong();
    geom.name = node.properties().at(1).toString();

    return geom;
}

FBXModel ParseModel(const FBXNode &node)
{
    auto props = node.getNodes("Properties70").at(0)->getNodes("P");

    FBXModel transform;
    transform.id = node.firstProperty()->asLong();
    transform.name = node.properties().at(1).toString();

    for (const auto &pNode : props)
    {
        if (pNode->properties().at(0).isString() &&
            pNode->properties().at(0).toString() == "Lcl Rotation")
        {
            glm::vec3 res;
            res.x = pNode->properties().at(4).toDouble();
            res.y = pNode->properties().at(5).toDouble();
            res.z = pNode->properties().at(6).toDouble();
            transform.rotation = res;
        }
    }

    for (const auto &pNode : props)
    {
        if (pNode->properties().at(0).isString() &&
            pNode->properties().at(0).toString() == "Lcl Translation")
        {
            glm::vec3 res;
            res.x = pNode->properties().at(4).toDouble();
            res.y = pNode->properties().at(5).toDouble();
            res.z = pNode->properties().at(6).toDouble();
            transform.translation = res;
        }
    }

    for (const auto &pNode : props)
    {
        if (pNode->properties().at(0).isString() &&
            pNode->properties().at(0).toString() == "Lcl Scaling")
        {
            glm::vec3 res;
            res.x = pNode->properties().at(4).toDouble();
            res.y = pNode->properties().at(5).toDouble();
            res.z = pNode->properties().at(6).toDouble();
            transform.scale = res;
        }
    }

    for (const auto &pNode : props)
    {
        if (pNode->properties().at(0).isString() &&
            pNode->properties().at(0).toString() == "GeometricTranslation")
        {
            glm::vec3 res;
            res.x = pNode->properties().at(4).toDouble();
            res.y = pNode->properties().at(5).toDouble();
            res.z = pNode->properties().at(6).toDouble();
            transform.geometricTranslation = res;
        }
    }

    return transform;
}

FBXConnection ParseConnection(const FBXNode &node)
{
    FBXConnection result;

    if (node.firstProperty()->isString())
    {
        auto oo = node.firstProperty()->toString();
        if (oo == "OO")
        {
            result.a = node.properties().at(1).asLong();
            result.b = node.properties().at(2).asLong();
        }
    }

    return result;
}

MeshResourceData* CreateResource(const std::vector<FBXGeometry> &geometries,
                                 const std::vector<FBXModel> &models,
                                 const std::vector<FBXConnection> &connections,
                                 Allocator &alloc)
{
    auto result = MAKE_NEW(alloc, MeshResourceData)();

    std::unordered_map<int64_t, std::vector<int64_t>> Map;

    for (const auto &geom : geometries)
    {
        for (auto c : connections)
        {
            if (c.a == geom.id)
                Map[geom.id].push_back(c.b);
        }
    }

    for (const auto &geom : geometries)
    {
        auto modelId = Map[geom.id].at(0);

        bool added = false;
        for (const auto &model : models)
        {
            if (model.id == modelId)
            {
                auto part = geom.part;

                auto vcount = part->vertexData.getVerticesCount();
                for (int j = 0; j < vcount; j++)
                {
                    auto vdata = (Vertex_p_n_tx_tan_bitan*)part->vertexData.getVertex(j);

                    vdata->pos += model.translation + model.geometricTranslation;
                }

                added = true;
            }
        }

        DF3D_ASSERT(added);

        result->parts.push_back(geom.part);
    }

    return result;
}

}

MeshResourceData* MeshLoader_fbx(ResourceDataSource &dataSource, Allocator &alloc)
{
    DF3D_ASSERT_MESS(false, "NOT IMPLEMENTED");

    auto root = FBXNode::FromFile(dataSource);
    if (!root)
        return nullptr;

    auto objects = root->getNodes("Objects");
    DF3D_ASSERT(objects.size() == 1);

    auto geometries = objects[0]->getNodes("Geometry");
    auto models = objects[0]->getNodes("Model");

    DF3D_ASSERT(geometries.size() == models.size());

    std::vector<FBXGeometry> fbxMeshes;
    for (auto geom : geometries)
        fbxMeshes.push_back(ParseGeometry(*geom, alloc));

    std::vector<FBXModel> fbxModels;
    for (auto model : models)
        fbxModels.push_back(ParseModel(*model));

    std::vector<FBXConnection> fbxConnections;
    auto connections = root->getNodes("Connections")[0]->getNodes("C");
    for (auto connection : connections)
    {
        auto conn = ParseConnection(*connection);
        if (conn.a != -1 && conn.b != -1)
            fbxConnections.push_back(conn);
    }

    return CreateResource(fbxMeshes, fbxModels, fbxConnections, alloc);
}

}

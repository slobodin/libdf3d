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

#pragma pack(push, 1)

struct FBXHeader
{
    uint8_t magic[21];
    uint16_t unknown;
    uint32_t version;
};

#pragma pack(pop)

static_assert(sizeof(FBXHeader) == 0x1b);

struct FBXProperty
{
    uint8_t typeCode;
    uint32_t offset;
};

struct FBXNode
{
    std::string name;
    std::vector<unique_ptr<FBXNode>> children;
    std::vector<FBXProperty> properties;
};

void Skip(ResourceDataSource &dataSource, int bytes)
{
    dataSource.seek(bytes, SeekDir::CURRENT);
}

unique_ptr<FBXNode> ReadNode(ResourceDataSource &dataSource)
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

    std::string name;
    for (uint8_t i = 0; i < nodeHeader.nameLen; i++)
    {
        char ch;
        dataSource.read(&ch, 1);
        name.push_back(ch);
    }

    auto fbxNode = make_unique<FBXNode>();
    fbxNode->name = std::move(name);

    for (uint32_t i = 0; i < nodeHeader.numProps; i++)
    {
        uint8_t typeCode;
        dataSource.read(&typeCode, 1);

        FBXProperty property;
        property.typeCode = typeCode;
        property.offset = dataSource.tell();
        fbxNode->properties.push_back(property);

        switch (typeCode)
        {
            // 16 bit int
            case 'Y':
                Skip(dataSource, 2);
                break;

            // 1 bit bool flag (yes/no)
            case 'C':
                Skip(dataSource, 1);
                break;

            // 32 bit int
            case 'I':
                Skip(dataSource, 4);
                break;

            // float
            case 'F':
                Skip(dataSource, 4);
                break;

            // double
            case 'D':
                Skip(dataSource, 8);
                break;

            // 64 bit int
            case 'L':
                Skip(dataSource, 8);
                break;

            // raw binary data
            case 'R':
            {
                uint32_t len;
                dataSource.read(&len, 4);

                Skip(dataSource, len);
            }
            break;

            // string
            case 'S':
            {
                uint32_t len;
                dataSource.read(&len, 4);

                Skip(dataSource, len);
            }
            break;

            // array of *
            case 'f':
            case 'd':
            case 'l':
            case 'i':
            {
                uint32_t arrayLen, encoding, compressedLen;

                dataSource.read(&arrayLen, 4);
                dataSource.read(&encoding, 4);
                dataSource.read(&compressedLen, 4);

                // compute length based on type and check against the stored value
                if (encoding == 0) 
                {
                    uint32_t stride = 0;
                    switch (typeCode)
                    {
                        case 'f':
                        case 'i':
                            stride = 4;
                            break;
                        case 'd':
                        case 'l':
                            stride = 8;
                            break;
                        default:
                            DF3D_ASSERT(false);
                    };

                    if (arrayLen * stride != compressedLen)
                    {
                        DF3D_ASSERT(false);
                    }
                }
                // zip/deflate algorithm (encoding==1)? take given length. anything else? die
                else if (encoding != 1)
                {
                    DF3D_ASSERT(false);
                }

                Skip(dataSource, compressedLen);

                break;
            }

            break;
            default:
                DF3D_ASSERT(false);
        }
    }

    while (dataSource.tell() < nodeHeader.endOffset)
    {
        auto child = ReadNode(dataSource);
        if (child)
            fbxNode->children.push_back(std::move(child));
    }

    return fbxNode;
}

unique_ptr<FBXNode> ParseFBX(ResourceDataSource &dataSource)
{
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

        rootFbxNode->children.push_back(std::move(node));
    }

    return rootFbxNode;
}

std::vector<uint8_t> DecompressBuffer(ResourceDataSource &dataSource, uint32_t arrayLen, uint32_t compressedLen, uint32_t uncompressedLength)
{
    uLongf destLen = uncompressedLength;
    uLong srcLen = compressedLen;

    std::vector<uint8_t> decompressedBuffer(uncompressedLength);
    std::vector<uint8_t> compressedBuffer(compressedLen);
    dataSource.read(compressedBuffer.data(), compressedLen);

    uncompress(decompressedBuffer.data(), &destLen, compressedBuffer.data(), srcLen);

    DF3D_ASSERT(srcLen == compressedLen);
    DF3D_ASSERT(destLen == uncompressedLength);

    return decompressedBuffer;
}

std::vector<double> ReadBufferDouble(ResourceDataSource &dataSource, uint32_t offset)
{
    dataSource.seek(offset, SeekDir::BEGIN);

    uint32_t arrayLen, encoding, compressedLen;

    dataSource.read(&arrayLen, 4);
    dataSource.read(&encoding, 4);
    dataSource.read(&compressedLen, 4);

    if (encoding == 1)
    {
        auto decompressedBuffer = DecompressBuffer(dataSource, arrayLen, compressedLen, arrayLen * 8);

        DF3D_ASSERT(decompressedBuffer.size() % 8 == 0);

        std::vector<double> result;
        for (int i = 0; i < decompressedBuffer.size(); i += 8)
        {
            union
            {
                uint8_t b[8];
                double d;
            } Val;

            memcpy(&Val, &decompressedBuffer[i], 8);

            result.push_back(Val.d);
        }

        return result;
    }
    else if (encoding == 0)
    {
        std::vector<double> buffer(arrayLen);
        dataSource.read(buffer.data(), compressedLen);

        return buffer;
    }

    DF3D_ASSERT(false);
    return {};
}

std::vector<int32_t> ReadBufferInt(ResourceDataSource &dataSource, uint32_t offset)
{
    dataSource.seek(offset, SeekDir::BEGIN);

    uint32_t arrayLen, encoding, compressedLen;

    dataSource.read(&arrayLen, 4);
    dataSource.read(&encoding, 4);
    dataSource.read(&compressedLen, 4);

    if (encoding != 0)
    {
        auto decompressedBuffer = DecompressBuffer(dataSource, arrayLen, compressedLen, arrayLen * 4);

        DF3D_ASSERT(decompressedBuffer.size() % 4 == 0);

        std::vector<int32_t> result;
        for (int i = 0; i < decompressedBuffer.size(); i += 4)
        {
            union
            {
                uint8_t b[4];
                int32_t i;
            } Val;

            memcpy(&Val, &decompressedBuffer[i], 4);

            result.push_back(Val.i);
        }

        return result;
    }

    std::vector<int32_t> buffer(arrayLen);
    dataSource.read(buffer.data(), compressedLen);

    return buffer;
}

std::string ReadString(ResourceDataSource &dataSource, int offset)
{
    dataSource.seek(offset, SeekDir::BEGIN);

    uint32_t len;
    dataSource.read(&len, 4);

    std::string res;
    for (int i = 0; i < len; i++)
    {
        char ch;
        dataSource.read(&ch, 1);
        res.push_back(ch);
    }
    return res;
}

std::vector<glm::vec3> ReadVertices(ResourceDataSource &dataSource, const FBXNode &node)
{
    DF3D_ASSERT(node.properties.size() == 1);
    DF3D_ASSERT(node.properties[0].typeCode == 'd');

    auto coords = ReadBufferDouble(dataSource, node.properties[0].offset);

    DF3D_ASSERT(coords.size() % 3 == 0);

    std::vector<glm::vec3> positions;
    for (int i = 0; i < coords.size(); i += 3)
    {
        float x = (float)coords[i + 0];
        float y = (float)coords[i + 1];
        float z = (float)coords[i + 2];
        positions.push_back({ x, y, z });
    }
    return positions;
}

std::vector<uint16_t> ReadIndices(ResourceDataSource &dataSource, const FBXNode &node)
{
    DF3D_ASSERT(node.properties.size() == 1);
    DF3D_ASSERT(node.properties[0].typeCode == 'i');

    auto coords = ReadBufferInt(dataSource, node.properties[0].offset);
    DF3D_ASSERT(coords.size() % 3 == 0);

    std::vector<uint16_t> result;
    for (auto i : coords)
    {
        DF3D_ASSERT(i < 0xFFFF);
        if (i < 0)
            i = -i - 1;

        result.push_back(i);
    }
    return result;
}

MeshResourceData::Part* ParseGeometry(ResourceDataSource &dataSource, const FBXNode &node, Allocator &alloc)
{
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> normals;
    std::vector<uint16_t> indices;

    for (const auto &child : node.children)
    {
        if (child->name == "Vertices")
            verts = ReadVertices(dataSource, *child);
        if (child->name == "PolygonVertexIndex")
            indices = ReadIndices(dataSource, *child);

        if (child->name == "LayerElementNormal")
        {
            for (const auto &tmp : child->children)
            {
                if (tmp->name == "MappingInformationType")
                {
                    auto mappingType = ReadString(dataSource, tmp->properties[0].offset);
                    DF3D_ASSERT(mappingType == "ByPolygonVertex");
                }
                if (tmp->name == "Normals")
                {
                    normals = ReadVertices(dataSource, *tmp);
                }
            }
        }
    }

    if (verts.empty())
        return nullptr;

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

    for (auto i : indices)
    {
        meshPart->indexData.push_back(i);
    }

    const auto vData = (Vertex_p_n_tx_tan_bitan*)meshPart->vertexData.getRawData();
    const auto vCount = meshPart->vertexData.getVerticesCount();
    MeshUtils::computeTangentBasis(vData, vCount);

    meshPart->materialName = "02___Default";

    return meshPart;
}

}

MeshResourceData* MeshLoader_fbx(ResourceDataSource &dataSource, Allocator &alloc)
{
    auto root = ParseFBX(dataSource);
    if (!root)
        return nullptr;

    auto result = MAKE_NEW(alloc, MeshResourceData)();
    for (const auto &node : root->children)
    {
        if (node->name == "Objects")
        {
            for (const auto &n1 : node->children)
            {
                if (n1->name == "Geometry")
                {
                    auto part = ParseGeometry(dataSource, *n1, alloc);
                    if (part)
                        result->parts.push_back(part);
                }
            }
        }
    }

    return result;
}

}

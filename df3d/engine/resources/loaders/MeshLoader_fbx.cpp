#include "MeshLoader_fbx.h"

#include <df3d/engine/render/MeshUtils.h>
#include <df3d/engine/EngineCVars.h>
#include <df3d/engine/resources/MeshResource.h>
#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/lib/Utils.h>

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

        switch (typeCode)
        {
            // 16 bit int
            case 'Y':
            {
                uint16_t value;
                dataSource.read(&value, 2);
            }
            break;

            // 1 bit bool flag (yes/no)
            case 'C':
            {
                uint8_t value;
                dataSource.read(&value, 1);
            }
            break;

            // 32 bit int
            case 'I':
            {
                uint32_t value;
                dataSource.read(&value, 4);
            }
            break;

            // float
            case 'F':
            {
                float value;
                dataSource.read(&value, 4);
            }
            break;

            // double
            case 'D':
            {
                double value;
                dataSource.read(&value, 8);
            }
            break;

            // 64 bit int
            case 'L':
            {
                uint64_t value;
                dataSource.read(&value, 8);
            }
            break;

            // raw binary data
            case 'R':
            {
                uint32_t len;
                dataSource.read(&len, 4);

                std::vector<uint8_t> value(len);
                dataSource.read(value.data(), len);
            }
            break;

            // string
            case 'S':
            {
                std::string value;
                uint32_t len;
                dataSource.read(&len, 4);

                for (uint32_t i = 0; i < len; i++)
                {
                    char ch;
                    dataSource.read(&ch, 1);
                    value.push_back(ch);
                }
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

                std::vector<uint8_t> value(compressedLen);
                dataSource.read(value.data(), compressedLen);

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

}

MeshResourceData* MeshLoader_fbx(ResourceDataSource &dataSource, Allocator &alloc)
{
    FBXHeader header;
    dataSource.read(&header, sizeof(FBXHeader));

    if (strncmp((const char *)header.magic, "Kaydara FBX Binary", 18)) {
        DFLOG_WARN("Invalid FBX magic");
        return nullptr;
    }

    auto rootFbxNode = make_unique<FBXNode>();

    while (dataSource.tell() < dataSource.getSize())
    {
        auto node = ReadNode(dataSource);
        if (!node)
            break;

        DFLOG_WARN("%s", node->name.c_str());

        rootFbxNode->children.push_back(std::move(node));
    }

    return nullptr;
}

}

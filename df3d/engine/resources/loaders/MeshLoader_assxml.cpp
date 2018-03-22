#include "MeshLoader_dfmesh.h"

#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/resources/MeshResource.h>
#include <df3d/engine/render/Vertex.h>
#include <df3d/engine/render/MeshUtils.h>
#include <pugixml/src/pugixml.hpp>

namespace df3d {

namespace {

pugi::xml_document ParseDoc(ResourceDataSource &dataSource)
{
    pugi::xml_document doc;

    std::string buffer;
    buffer.resize(dataSource.getSize());
    dataSource.read(buffer.data(), buffer.size());

    pugi::xml_parse_result result = doc.load_buffer(buffer.data(), buffer.size());
    DF3D_ASSERT(result);

    return doc;
}

std::vector<glm::vec3> ReadArrayVec3(pugi::xml_node n)
{
    auto numValues = n.attribute("num").as_int();
    auto numComponents = n.attribute("num_components").as_int();

    DF3D_ASSERT(numComponents == 3);

    std::vector<glm::vec3> result;
    result.reserve(numValues);

    auto data = n.first_child().value();

    for (int i = 0; i < numValues; i++)
    {
        glm::vec3 v;
        int offset;
        auto read = sscanf(data, "%f %f %f%n", &v.x, &v.y, &v.z, &offset);

        DF3D_ASSERT(read == 3);

        result.push_back(v);
        data += offset;
    }

    return result;
}

std::vector<glm::vec2> ReadArrayVec2(pugi::xml_node n)
{
    auto numValues = n.attribute("num").as_int();
    auto numComponents = n.attribute("num_components").as_int();

    DF3D_ASSERT(numComponents == 2);

    std::vector<glm::vec2> result;
    result.reserve(numValues);

    auto data = n.first_child().value();

    for (int i = 0; i < numValues; i++)
    {
        glm::vec2 v;
        int offset;
        auto read = sscanf(data, "%f %f%n", &v.x, &v.y, &offset);

        DF3D_ASSERT(read == 2);

        result.push_back(v);
        data += offset;
    }

    return result;
}

df3d::PodArray<uint16_t> ReadFaceList(pugi::xml_node n, Allocator &alloc)
{
    df3d::PodArray<uint16_t> result(alloc);

    for (pugi::xml_node faceNode = n.child("Face"); faceNode; faceNode = faceNode.next_sibling("Face"))
    {
        auto num = faceNode.attribute("num").as_int();
        auto data = faceNode.first_child().value();

        DF3D_ASSERT(num == 3);

        glm::ivec3 idx;

        sscanf(data, "%d %d %d", &idx.x, &idx.y, &idx.z);

        DF3D_ASSERT(idx.x < 0xFFFF && idx.y < 0xFFFF && idx.z < 0xFFFF);

        result.push_back(idx.x);
        result.push_back(idx.y);
        result.push_back(idx.z);
    }

    return result;
}

MeshResourceData* ParseMeshes(pugi::xml_node meshListNode, Allocator &alloc)
{
    auto result = MAKE_NEW(alloc, MeshResourceData)();

    for (pugi::xml_node meshNode = meshListNode.child("Mesh"); meshNode; meshNode = meshNode.next_sibling("Mesh"))
    {
        auto positions = ReadArrayVec3(meshNode.child("Positions"));
        auto normals = ReadArrayVec3(meshNode.child("Normals"));
        auto txCoords = ReadArrayVec2(meshNode.child("TextureCoords"));

        DF3D_ASSERT(positions.size() == normals.size() && normals.size() == txCoords.size());

        auto vf = Vertex_p_n_tx_tan_bitan::getFormat();

        auto meshPart = MAKE_NEW(alloc, MeshResourceData::Part)(vf, alloc);
        meshPart->indicesType = INDICES_16_BIT;
        meshPart->materialName = "02___Default";
        meshPart->indexData = ReadFaceList(meshNode.child("FaceList"), alloc);

        for (int i = 0; i < positions.size(); i++)
        {
            meshPart->vertexData.addVertex();
            auto v = (Vertex_p_n_tx_tan_bitan*)meshPart->vertexData.getVertex(i);

            v->pos = positions[i];
            v->normal = normals[i];
            v->uv = txCoords[i];
            v->tangent = {};
            v->bitangent = {};
        }

        const auto vData = (Vertex_p_n_tx_tan_bitan*)meshPart->vertexData.getRawData();
        const auto vCount = meshPart->vertexData.getVerticesCount();
        MeshUtils::computeTangentBasis(vData, vCount);

        result->parts.push_back(meshPart);
    }

    return result;
}

}

MeshResourceData* MeshLoader_assxml(ResourceDataSource &dataSource, Allocator &alloc)
{
    auto doc = ParseDoc(dataSource);

    auto mesh = ParseMeshes(doc.child("ASSIMP").child("Scene").child("MeshList"), alloc);

    return mesh;
}

}

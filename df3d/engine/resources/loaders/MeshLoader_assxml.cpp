#include "MeshLoader_dfmesh.h"

#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/resources/MeshResource.h>
#include <df3d/engine/render/Vertex.h>
#include <df3d/engine/render/MeshUtils.h>
#include <df3d/lib/Utils.h>
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

glm::mat4 ParseMatrix(const char *data)
{
    glm::vec4 row1, row2, row3, row4;
    sscanf(data, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
            &row1.x, &row1.y, &row1.z, &row1.w,
            &row2.x, &row2.y, &row2.z, &row2.w,
            &row3.x, &row3.y, &row3.z, &row3.w,
            &row4.x, &row4.y, &row4.z, &row4.w);

    auto m = glm::mat4(row1, row2, row3, row4);

    return glm::transpose(m);
}

glm::mat4 ReadTransformRecursive(pugi::xml_node n)
{
    if (!n.child("MeshRefs").empty())
        return glm::mat4(1.0f);

    auto children = n.child("NodeList");
    DF3D_ASSERT(children.attribute("num").as_int() == 1);

    auto myMatrix = ParseMatrix(n.child("Matrix4").first_child().value());
    auto childMatrix = ReadTransformRecursive(children.child("Node"));

    return myMatrix * childMatrix;
}

std::vector<shared_ptr<MeshResourceData::Part>> ParseMeshes(pugi::xml_node meshListNode, Allocator &alloc)
{
    std::vector<shared_ptr<MeshResourceData::Part>> result;

    for (pugi::xml_node meshNode = meshListNode.child("Mesh"); meshNode; meshNode = meshNode.next_sibling("Mesh"))
    {
        auto positions = ReadArrayVec3(meshNode.child("Positions"));
        auto normals = ReadArrayVec3(meshNode.child("Normals"));
        auto txCoords = ReadArrayVec2(meshNode.child("TextureCoords"));

        DF3D_ASSERT(positions.size() == normals.size() && normals.size() == txCoords.size());

        auto vf = Vertex_p_n_tx_tan_bitan::getFormat();

        auto meshPart = make_shared<MeshResourceData::Part>(vf, alloc);
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

        result.push_back(meshPart);
    }

    return result;
}

shared_ptr<AnimatedMeshNode> ParseNodes(pugi::xml_node rootNode)
{
    auto result = make_shared<AnimatedMeshNode>();

    result->transform = ParseMatrix(rootNode.child("Matrix4").first_child().value());
    result->name = rootNode.attribute("name").as_string();

    if (!rootNode.child("MeshRefs").empty())
    {
        auto val = rootNode.child("MeshRefs").first_child().value();
        result->meshIdx = df3d::utils::from_string<int>(val);
    }

    auto nodeList = rootNode.child("NodeList");
    for (pugi::xml_node n = nodeList.child("Node"); n; n = n.next_sibling("Node"))
    {
        auto child = ParseNodes(n);
        result->children.push_back(child);
    }

    return result;
}

std::vector<glm::vec3> ParseAnimationPositions(pugi::xml_node root)
{
    std::vector<glm::vec3> result;

    for (pugi::xml_node n = root.child("PositionKey"); n; n = n.next_sibling("PositionKey"))
    {
        glm::vec3 value;

        sscanf(n.first_child().value(), "%f %f %f", &value.x, &value.y, &value.z);

        result.push_back(value);
    }

    return result;
}

std::vector<glm::quat> ParseAnimationOrientations(pugi::xml_node root)
{
    std::vector<glm::quat> result;

    for (pugi::xml_node n = root.child("RotationKey"); n; n = n.next_sibling("RotationKey"))
    {
        glm::quat value;

        sscanf(n.first_child().value(), "%f %f %f %f", &value.x, &value.y, &value.z, &value.w);

        result.push_back(value);
    }

    return result;
}

using AnimationMap = std::unordered_map<std::string, std::vector<AnimationFrameData>>;

AnimationMap ParseAnimation(pugi::xml_node animListNode)
{
    std::unordered_map<std::string, std::vector<AnimationFrameData>> result;

    for (pugi::xml_node n = animListNode.child("NodeAnim"); n; n = n.next_sibling("NodeAnim"))
    {
        std::string nodeName = n.attribute("node").as_string();

        auto positions = ParseAnimationPositions(n.child("PositionKeyList"));
        auto rotations = ParseAnimationOrientations(n.child("RotationKeyList"));

        std::vector<AnimationFrameData> frames;

        auto size = std::max(positions.size(), rotations.size());
        for (size_t i = 0; i < size; i++)
        {
            AnimationFrameData frame;
            if (i < positions.size())
                frame.position = positions[i];
            if (i < rotations.size())
                frame.orientation = rotations[i];

            frames.push_back(frame);
        }

        DF3D_ASSERT(!df3d::utils::contains_key(result, nodeName));

        result[nodeName] = frames;
    }

    return result;
}

void SetAnimation(const AnimationMap &animMap, AnimatedMeshNode *node)
{
    auto found = animMap.find(node->name);
    if (found != animMap.end())
        node->animation = found->second;

    for (const auto &child : node->children)
        SetAnimation(animMap, child.get());
}

}

AnimatedMeshResourceData* MeshLoader_assxml(ResourceDataSource &dataSource, Allocator &alloc)
{
    auto doc = ParseDoc(dataSource);

    auto result = MAKE_NEW(alloc, AnimatedMeshResourceData)();

    result->parts = ParseMeshes(doc.child("ASSIMP").child("Scene").child("MeshList"), alloc);
    result->root = ParseNodes(doc.child("ASSIMP").child("Scene").child("Node"));

    auto animations = ParseAnimation(doc.child("ASSIMP").child("Scene").child("AnimationList").child("Animation").child("NodeAnimList"));

    SetAnimation(animations, result->root.get());

    return result;
}

}

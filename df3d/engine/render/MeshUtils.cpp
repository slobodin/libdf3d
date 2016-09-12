#include "MeshUtils.h"

#include <df3d/lib/math/MathUtils.h>
#include <df3d/engine/render/MeshData.h>
#include <df3d/engine/EngineController.h>

namespace df3d {

static bool IsNear(float v1, float v2)
{
    return std::abs(v1 - v2) < glm::epsilon<float>();
}

struct CompareVertices
{
    bool operator()(const Vertex_p_n_tx_tan_bitan &a, const Vertex_p_n_tx_tan_bitan &b) const
    {
        return memcmp(&a, &b, sizeof(Vertex_p_n_tx_tan_bitan)) > 0;
    }
};

static void OrthogonalizeAndFixHandedness(Vertex_p_n_tx_tan_bitan *vdata, size_t verticesCount)
{
    for (size_t i = 0; i < verticesCount; i++)
    {
        auto &v = vdata[i];

        // Gram-Schmidt orthogonalization.
        v.tangent = v.tangent - v.normal * glm::dot(v.normal, v.tangent);
        v.tangent = MathUtils::safeNormalize(v.tangent);

        // Fixe handedness (right-handed).
        if (glm::dot(glm::cross(v.normal, v.tangent), v.bitangent) < 0.0f)
            v.tangent = v.tangent * -1.0f;
    }
}

static void CalcTangentSpaceTriangle(Vertex_p_n_tx_tan_bitan &v0, Vertex_p_n_tx_tan_bitan &v1, Vertex_p_n_tx_tan_bitan &v2)
{
    // Lengyel, Eric. "Computing Tangent Space Basis Vectors for an Arbitrary Mesh"

    float x1 = v1.pos.x - v0.pos.x;
    float x2 = v2.pos.x - v0.pos.x;
    float y1 = v1.pos.y - v0.pos.y;
    float y2 = v2.pos.y - v0.pos.y;
    float z1 = v1.pos.z - v0.pos.z;
    float z2 = v2.pos.z - v0.pos.z;

    float s1 = v1.uv.x - v0.uv.x;
    float s2 = v2.uv.x - v0.uv.x;
    float t1 = v1.uv.y - v0.uv.y;
    float t2 = v2.uv.y - v0.uv.y;

    float r = 1.0f / (s1 * t2 - s2 * t1);
    glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
    glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

    v0.tangent += sdir;
    v1.tangent += sdir;
    v2.tangent += sdir;

    v0.bitangent += tdir;
    v1.bitangent += tdir;
    v2.bitangent += tdir;
}

static bool FindSimilarVertexIndex(const Vertex_p_n_tx_tan_bitan &vertex, 
                                   const std::map<Vertex_p_n_tx_tan_bitan, uint32_t, CompareVertices> &lookup,
                                   uint32_t &result)
{
    auto found = lookup.find(vertex);
    if (found != lookup.end())
    {
        result = found->second;
        return true;
    }
    return false;
}

void MeshUtils::indexize(const Vertex_p_n_tx_tan_bitan *vdata, size_t count,
                         PodArray<Vertex_p_n_tx_tan_bitan> &outVertices, PodArray<uint32_t> &outIndices)
{
    std::map<Vertex_p_n_tx_tan_bitan, uint32_t, CompareVertices> lookup;

    for (size_t i = 0; i < count; i++)
    {
        uint32_t index;

        if (FindSimilarVertexIndex(vdata[i], lookup, index))
        {
            outIndices.push_back(index);
        }
        else
        {
            outVertices.push_back(vdata[i]);

            auto newIdx = (uint32_t)outVertices.size() - 1;
            outIndices.push_back(newIdx);
            lookup[vdata[i]] = newIdx;
        }
    }
}

void MeshUtils::computeTangentBasis(Vertex_p_n_tx_tan_bitan *vdata, size_t count)
{
    for (size_t i = 0; i < count; i += 3)
    {
        auto &v0 = vdata[i + 0];
        auto &v1 = vdata[i + 1];
        auto &v2 = vdata[i + 2];

        CalcTangentSpaceTriangle(v0, v1, v2);
    }

    OrthogonalizeAndFixHandedness(vdata, count);
}

void MeshUtils::computeTangentBasis(Vertex_p_n_tx_tan_bitan *vdata, size_t verticesCount,
                                    const uint32_t *indices, size_t indicesCount)
{
    for (size_t i = 0; i < indicesCount; i += 3)
    {
        auto idx1 = indices[i + 0];
        auto idx2 = indices[i + 1];
        auto idx3 = indices[i + 2];

        auto &v0 = vdata[idx1];
        auto &v1 = vdata[idx2];
        auto &v2 = vdata[idx3];

        CalcTangentSpaceTriangle(v0, v1, v2);
    }

    OrthogonalizeAndFixHandedness(vdata, verticesCount);

    // TODO: smooth.
}

}

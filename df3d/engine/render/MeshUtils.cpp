#include "MeshUtils.h"

#include <df3d/lib/math/MathUtils.h>
#include <df3d/engine/render/MeshData.h>
#include <df3d/engine/EngineController.h>
#include <mikktspace/mikktspace.h>

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

struct MikkTSpaceUserData
{
    Vertex_p_n_tx_tan_bitan *vdata;
    size_t verticesCount;
};

struct MikkTSpaceInterface
{
    // Returns the number of faces (triangles/quads) on the mesh to be processed.
    static int getNumFaces(const SMikkTSpaceContext *pContext)
    {
        return ((MikkTSpaceUserData*)pContext->m_pUserData)->verticesCount / 3;
    }

    // Returns the number of vertices on face number iFace
    // iFace is a number in the range {0, 1, ..., getNumFaces()-1}
    static int getNumVerticesOfFace(const SMikkTSpaceContext *pContext, const int iFace)
    {
        return 3;
    }

    // returns the position/normal/texcoord of the referenced face of vertex number iVert.
    // iVert is in the range {0,1,2} for triangles and {0,1,2,3} for quads.
    static void getPosition(const SMikkTSpaceContext *pContext, float fvPosOut[], const int iFace, const int iVert)
    {
        auto myData = (MikkTSpaceUserData*)pContext->m_pUserData;
        const auto &pos = myData->vdata[iFace * 3 + iVert].pos;
        fvPosOut[0] = pos.x;
        fvPosOut[1] = pos.y;
        fvPosOut[2] = pos.z;
    }

    static void getNormal(const SMikkTSpaceContext *pContext, float fvNormOut[], const int iFace, const int iVert)
    {
        auto myData = (MikkTSpaceUserData*)pContext->m_pUserData;
        const auto &normal = myData->vdata[iFace * 3 + iVert].normal;
        fvNormOut[0] = normal.x;
        fvNormOut[1] = normal.y;
        fvNormOut[2] = normal.z;
    }

    static void getTexCoord(const SMikkTSpaceContext *pContext, float fvTexcOut[], const int iFace, const int iVert)
    {
        auto myData = (MikkTSpaceUserData*)pContext->m_pUserData;
        const auto &tx = myData->vdata[iFace * 3 + iVert].uv;
        fvTexcOut[0] = tx.x;
        fvTexcOut[1] = tx.y;
    }

    // This function is used to return the tangent and fSign to the application.
    // fvTangent is a unit length vector.
    // For normal maps it is sufficient to use the following simplified version of the bitangent which is generated at pixel/vertex level.
    // bitangent = fSign * cross(vN, tangent);
    // Note that the results are returned unindexed. It is possible to generate a new index list
    // But averaging/overwriting tangent spaces by using an already existing index list WILL produce INCRORRECT results.
    // DO NOT! use an already existing index list.
    static void setTSpaceBasic(const SMikkTSpaceContext *pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert)
    {
        auto myData = (MikkTSpaceUserData*)pContext->m_pUserData;
        auto &vert = myData->vdata[iFace * 3 + iVert];
        vert.tangent.x = fvTangent[0];
        vert.tangent.y = fvTangent[1];
        vert.tangent.z = fvTangent[2];
        vert.bitangent = fSign * glm::cross(vert.normal, vert.tangent);
    }
};

static void OrthogonalizeAndFixHandedness(Vertex_p_n_tx_tan_bitan *vdata, size_t verticesCount)
{
    for (size_t i = 0; i < verticesCount; i++)
    {
        auto &v = vdata[i];

        // Gram-Schmidt orthogonalization.
        v.tangent = v.tangent - v.normal * glm::dot(v.normal, v.tangent);

        float magT = glm::length(v.tangent);
        float magB = glm::length(v.bitangent);

        v.tangent = MathUtils::safeNormalize(v.tangent);
        v.bitangent = MathUtils::safeNormalize(v.bitangent);

        // Reconstruct degenerate case, taken from here:
        // https://gist.github.com/aras-p/2843984#file-importmeshutility-cpp

        if (magT < glm::epsilon<float>() || magB < glm::epsilon<float>())
        {
            glm::vec3 axis1, axis2;
            const glm::vec3 AxisX = { 1.0f, 0.0f, 0.0f };
            const glm::vec3 AxisY = { 0.0f, 1.0f, 0.0f };
            const glm::vec3 AxisZ = { 0.0f, 0.0f, 1.0f };

            float dpXN = std::abs(glm::dot(AxisX, v.normal));
            float dpYN = std::abs(glm::dot(AxisY, v.normal));
            float dpZN = std::abs(glm::dot(AxisZ, v.normal));

            if (dpXN <= dpYN && dpXN <= dpZN)
            {
                axis1 = AxisX;
                if (dpYN <= dpZN)
                    axis2 = AxisY;
                else
                    axis2 = AxisZ;
            }
            else if (dpYN <= dpXN && dpYN <= dpZN)
            {
                axis1 = AxisY;
                if (dpXN <= dpZN)
                    axis2 = AxisX;
                else
                    axis2 = AxisZ;
            }
            else
            {
                axis1 = AxisZ;
                if (dpXN <= dpYN)
                    axis2 = AxisX;
                else
                    axis2 = AxisY;
            }

            v.tangent = axis1 - glm::dot(v.normal, axis1) * v.normal;
            v.bitangent = axis2 - glm::dot(v.normal, axis2) * v.normal - glm::dot(v.tangent, axis2) * MathUtils::safeNormalize(v.tangent);

            v.tangent = MathUtils::safeNormalize(v.tangent);
            v.bitangent = MathUtils::safeNormalize(v.bitangent);
        }

        // Fix handedness (right-handed).
        if (glm::dot(glm::cross(v.normal, v.tangent), v.bitangent) < 0.0f)
            v.tangent = -v.tangent;
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
    MikkTSpaceUserData myData;
    myData.vdata = vdata;
    myData.verticesCount = count;

    SMikkTSpaceInterface interface;
    interface.m_getNumFaces = MikkTSpaceInterface::getNumFaces;
    interface.m_getNumVerticesOfFace = MikkTSpaceInterface::getNumVerticesOfFace;
    interface.m_getPosition = MikkTSpaceInterface::getPosition;
    interface.m_getNormal = MikkTSpaceInterface::getNormal;
    interface.m_getTexCoord = MikkTSpaceInterface::getTexCoord;
    interface.m_setTSpaceBasic = MikkTSpaceInterface::setTSpaceBasic;
    interface.m_setTSpace = nullptr;

    SMikkTSpaceContext context;
    context.m_pInterface = &interface;
    context.m_pUserData = &myData;

    if (!genTangSpaceDefault(&context))
        DFLOG_WARN("Failed to calculate tangent space!");
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

    // TODO: smooth tangents.

    OrthogonalizeAndFixHandedness(vdata, verticesCount);
}

}

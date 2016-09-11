#include "MeshUtils.h"

#include <df3d/lib/math/MathUtils.h>
#include <df3d/engine/render/MeshData.h>
#include <df3d/engine/EngineController.h>

namespace df3d {

static bool IsNear(float v1, float v2)
{
    return std::abs(v1 - v2) < 0.0001f;
}

static bool FindSimilarVertexIndex(const Vertex_p_n_tx_tan_bitan &vertex, 
                                   PodArray<Vertex_p_n_tx_tan_bitan> &vertices,
                                   uint32_t &result)
{
    // Lame linear search
    for (size_t i = 0; i < vertices.size(); i++)
    {
        if (IsNear(vertex.pos.x, vertices[i].pos.x) &&
            IsNear(vertex.pos.y, vertices[i].pos.y) &&
            IsNear(vertex.pos.z, vertices[i].pos.z) &&
            IsNear(vertex.uv.x, vertices[i].uv.x) &&
            IsNear(vertex.uv.y, vertices[i].uv.y) &&
            IsNear(vertex.normal.x, vertices[i].normal.x) &&
            IsNear(vertex.normal.y, vertices[i].normal.y) &&
            IsNear(vertex.normal.z, vertices[i].normal.z))
        {
            result = i;
            return true;
        }
    }

    return false;
}

void MeshUtils::indexize(Vertex_p_n_tx_tan_bitan *vdata, size_t count,
                         PodArray<Vertex_p_n_tx_tan_bitan> &outVertices, PodArray<uint32_t> &outIndices)
{
    for (size_t i = 0; i < count; i++)
    {
        uint32_t index;
        bool found = FindSimilarVertexIndex(vdata[i], outVertices, index);

        if (found)
        {
            outIndices.push_back(index);

            // Average the tangents and the bitangents
            outVertices[index].tangent += vdata[i].tangent;
            outVertices[index].bitangent += vdata[i].bitangent;
        }
        else
        {
            outVertices.push_back(vdata[i]);
            outIndices.push_back((uint16_t)outVertices.size() - 1);
        }
    }

    for (auto &v : outVertices)
    {
        v.tangent = MathUtils::safeNormalize(v.tangent);
        v.bitangent = MathUtils::safeNormalize(v.bitangent);
    }
}

/*
void MeshUtils::computeNormals(SubMesh &submesh)
{
    DF3D_ASSERT_MESS(false, "Not implemented");

    const auto &vformat = submesh.getVertexData().getFormat();

    if (!vformat.hasAttribute(VertexFormat::NORMAL_3) || !vformat.hasAttribute(VertexFormat::POSITION_3))
        return;

    auto &vertexData = submesh.getVertexData();
    PodArray<int> polysTouchVertex(MemoryManager::allocDefault());
    polysTouchVertex.resize(vertexData.getVerticesCount());

    // Clear normals for all vertices.
    for (size_t i = 0; i < vertexData.getVerticesCount(); i++)
    {
        auto v = vertexData.getVertex(i);
        v.setNormal({ 0.0f, 0.0f, 0.0f });
    }

    // Indexed.
    if (submesh.hasIndices())
    {
        const auto &indices = submesh.getIndices();
        for (size_t ind = 0; ind < indices.size(); ind += 3)
        {
            size_t vindex0 = indices[ind];
            size_t vindex1 = indices[ind + 1];
            size_t vindex2 = indices[ind + 2];

            auto v1 = vertexData.getVertex(vindex0);
            auto v2 = vertexData.getVertex(vindex1);
            auto v3 = vertexData.getVertex(vindex2);

            polysTouchVertex[vindex0]++;
            polysTouchVertex[vindex1]++;
            polysTouchVertex[vindex2]++;

            glm::vec3 v0p, v1p, v2p;
            v1.getPosition(&v0p);
            v2.getPosition(&v1p);
            v3.getPosition(&v2p);

            glm::vec3 u = v1p - v0p;
            glm::vec3 v = v2p - v0p;

            // FIXME:
            // u x v depends on winding order
            glm::vec3 normal = glm::cross(u, v);
            // uv?

            glm::vec3 v1n, v2n, v3n;

            v1.getNormal(&v1n);
            v1n += normal;
            v1.setNormal(v1n);

            v2.getNormal(&v2n);
            v2n += normal;
            v2.setNormal(v2n);

            v2.getNormal(&v3n);
            v3n += normal;
            v2.setNormal(v3n);
        }

        for (size_t vertex = 0; vertex < vertexData.getVerticesCount(); vertex++)
        {
            if (polysTouchVertex[vertex] >= 1)
            {
                auto v = vertexData.getVertex(vertex);
                glm::vec3 n;
                v.getNormal(&n);

                n /= polysTouchVertex[vertex];

                n = MathUtils::safeNormalize(n);

                v.setNormal(n);
            }
        }
    }
    else
    {
        DFLOG_WARN("Cannot compute normals for triangle list mesh type.");
    }

}    */

void MeshUtils::computeTangentBasis(Vertex_p_n_tx_tan_bitan *vdata, size_t count)
{
    for (size_t i = 0; i < count; i += 3)
    {
        auto &v0 = vdata[i + 0];
        auto &v1 = vdata[i + 1];
        auto &v2 = vdata[i + 2];

        auto e1 = v1.pos - v0.pos;
        auto e2 = v2.pos - v0.pos;
        auto e1uv = v1.uv - v0.uv;
        auto e2uv = v2.uv - v0.uv;

        float r = 1.0f / (e1uv.x * e2uv.y - e1uv.y * e2uv.x);
        glm::vec3 tangent = (e1 * e2uv.y - e2 * e1uv.y) * r;
        glm::vec3 bitangent = (e2 * e1uv.x - e1 * e2uv.x) * r;

        v0.tangent = v1.tangent = v2.tangent = tangent;
        v0.bitangent = v1.bitangent = v2.bitangent = bitangent;
    }

    for (size_t i = 0; i < count; i++)
    {
        auto &v = vdata[i];

        // Gram-Schmidt orthogonalization.
        v.tangent = v.tangent - v.normal * glm::dot(v.normal, v.tangent);
        v.tangent = MathUtils::safeNormalize(v.tangent);

        if (glm::dot(glm::cross(v.normal, v.tangent), v.bitangent) < 0.0f)
            v.tangent = v.tangent * -1.0f;
    }
}

}

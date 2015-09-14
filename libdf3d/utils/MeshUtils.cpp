#include "df3d_pch.h"
#include "MeshUtils.h"

#include <utils/Utils.h>
#include <render/MeshData.h>

namespace df3d { namespace utils { namespace mesh { 

void indexize()
{
    // TODO:
    assert(false);

    //std::map<render::Vertex, render::INDICES_TYPE> alreadyIndexed;
    //render::IndexArray indexBuffer;
    //render::VertexArray newVertexBuffer;
    //const auto &vertices = vb->getVertices();

    //for (auto &v : vertices)
    //{
    //    auto found = alreadyIndexed.find(v);
    //    if (found != alreadyIndexed.end())
    //    {
    //        indexBuffer.push_back(found->second);
    //        newVertexBuffer.at(found->second).tangent += v.tangent;
    //        newVertexBuffer.at(found->second).bitangent += v.bitangent;
    //    }
    //    else
    //    {
    //        newVertexBuffer.push_back(v);
    //        render::INDICES_TYPE newIdx = newVertexBuffer.size() - 1;
    //        indexBuffer.push_back(newIdx);

    //        alreadyIndexed[v] = newIdx;
    //    }
    //}

    //auto ib = make_shared<render::IndexBuffer>();

    //ib->appendIndices(indexBuffer);
    //vb->getVertices().swap(newVertexBuffer);

    //ib->setDirty();
    //vb->setDirty();

    //return ib;
}

void computeNormals(render::SubMesh &submesh)
{
    const auto &vformat = submesh.getVertexFormat();

    if (!vformat.hasAttribute(render::VertexFormat::NORMAL_3) || !vformat.hasAttribute(render::VertexFormat::POSITION_3))
        return;

    auto &vertexData = submesh.getVertexData();
    std::vector<int> polysTouchVertex(vertexData.getVerticesCount());

    // Clear normals for all vertices.
    for (size_t i = 0; i < vertexData.getVerticesCount(); i++)
    {
        auto v = vertexData.getVertex(i);
        v.setNormal({ 0.0f, 0.0f, 0.0f });
    }

    // Indexed.
    if (submesh.hasIndices())
    {
        assert(false && "please check it works");

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

                n = utils::math::safeNormalize(n);

                v.setNormal(n);
            }
        }
    }
    else
    {
        base::glog << "Cannot compute normals for triangle list mesh type." << base::logwarn;
    }
}

void computeTangentBasis(render::SubMesh &submesh)
{
    const auto &format = submesh.getVertexFormat();
    if (!format.hasAttribute(render::VertexFormat::TANGENT_3) ||
        !format.hasAttribute(render::VertexFormat::BITANGENT_3) ||
        !format.hasAttribute(render::VertexFormat::POSITION_3) ||
        !format.hasAttribute(render::VertexFormat::TX_2) ||
        !format.hasAttribute(render::VertexFormat::NORMAL_3))
        return;

    // Indexed.
    if (submesh.hasIndices())
    {
        //std::vector<glm::vec3> tempTangent(m_vertices.size());
        //std::vector<glm::vec3> tempBinormal(m_vertices.size());

        //for (size_t i = 0; i < m_indices.size(); i += 3)
        //{
        //    size_t vindex0 = m_indices[i];
        //    size_t vindex1 = m_indices[i + 1];
        //    size_t vindex2 = m_indices[i + 2];

        //    const render::Vertex &v0 = m_vertices[vindex0];
        //    const render::Vertex &v1 = m_vertices[vindex1];
        //    const render::Vertex &v2 = m_vertices[vindex2];

        //    auto q1 = v1.p - v0.p;
        //    auto q2 = v2.p - v0.p;

        //    float s1 = v1.t.x - v0.t.x;
        //    float s2 = v2.t.x - v0.t.x;
        //    float t1 = v1.t.y - v0.t.y;
        //    float t2 = v2.t.y - v0.t.y;

        //    auto tangent = t2 * q1 - t1 * q2;
        //    SafeNormalize(tangent);

        //    auto bitangent = -s2 * q1 + s1 * q2;
        //    SafeNormalize(bitangent);

        //    for (size_t j = 0; j < 3; j++)
        //    {
        //        tempTangent.at(m_indices[j]) += tangent;
        //        tempBinormal.at(m_indices[j]) += bitangent;
        //    }
        //}

        //for (size_t i = 0; i < m_vertices.size(); i++)
        //{
        //    auto t = tempTangent[i];
        //    render::Vertex &v = m_vertices[i];

        //    t -= v.n * glm::dot(t, v.n);
        //    SafeNormalize(t);

        //    v.tangent = glm::vec3(t, 1.0f);

        //    if (glm::dot(glm::cross(v.n, t), tempBinormal[i]) < 0.0f)
        //        v.tangent.w = -1.0f;
        //    else
        //        v.tangent.w = 1.0f;
        //}
        base::glog << "Can not compute tangent space basis for indexed mesh." << base::logwarn;
    }
    else
    {
        auto &vertexData = submesh.getVertexData();

        for (size_t i = 0; i < vertexData.getVerticesCount(); i += 3)
        {
            auto v0 = vertexData.getVertex(i + 0);
            auto v1 = vertexData.getVertex(i + 1);
            auto v2 = vertexData.getVertex(i + 2);

            glm::vec3 v0p, v1p, v2p;
            v0.getPosition(&v0p);
            v1.getPosition(&v1p);
            v2.getPosition(&v2p);

            glm::vec2 v0t, v1t, v2t;
            v0.getTx(&v0t);
            v1.getTx(&v1t);
            v2.getTx(&v2t);

            auto e1 = v1p - v0p;
            auto e2 = v2p - v0p;
            auto e1uv = v1t - v0t;
            auto e2uv = v2t - v0t;

            float r = 1.0f / (e1uv.x * e2uv.y - e1uv.y * e2uv.x);
            glm::vec3 tangent = (e1 * e2uv.y - e2 * e1uv.y) * r;
            glm::vec3 bitangent = (e2 * e1uv.x - e1 * e2uv.x) * r;

            for (size_t j = 0; j < 3; j++)
            {
                auto v = vertexData.getVertex(i + j);
                v.setTangent(tangent);
                v.setBitangent(bitangent);
            }
        }

        // Gram-Schmidt orthogonalization.
        for (size_t i = 0; i < vertexData.getVerticesCount(); i++)
        {
            auto v = vertexData.getVertex(i);

            glm::vec3 tangent, bitangent, normal;
            v.getTangent(&tangent);
            v.getBitangent(&bitangent);
            v.getNormal(&normal);

            tangent = tangent - normal * glm::dot(normal, tangent);
            tangent = utils::math::safeNormalize(tangent);

            if (glm::dot(glm::cross(normal, tangent), bitangent) < 0.0f)
                tangent = tangent * -1.0f;

            v.setTangent(tangent);
        }
    }
}

} } }

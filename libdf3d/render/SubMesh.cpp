#include "df3d_pch.h"
#include "SubMesh.h"

#include "VertexIndexBuffer.h"
#include <utils/Utils.h>

namespace df3d { namespace render {

SubMesh::SubMesh()
{

}

void SubMesh::computeNormals()
{
    if (!m_vb || !m_vb->getFormat().hasComponent(VertexComponent::NORMAL) || !m_vb->getFormat().hasComponent(VertexComponent::POSITION))
        return;

    auto vertexData = m_vb->getVertexData();
    size_t stride = m_vb->getFormat().getVertexSize() / sizeof(float);
    size_t normalOffset = m_vb->getFormat().getOffsetTo(VertexComponent::NORMAL) / sizeof(float);
    size_t positionOffset = m_vb->getFormat().getOffsetTo(VertexComponent::POSITION) / sizeof(float);

    std::vector<int> polysTouchVertex(m_vb->getVerticesCount());

    // Clear normals for all vertices.
    for (size_t i = 0; i < m_vb->getVerticesCount(); i++)
    {
        float *normal = vertexData + i * stride + normalOffset;
        normal[0] = normal[1] = normal[2] = 0.0f;
    }

    // Indexed.
    if (m_ib)
    {
        auto indices = m_ib->getIndices();
        for (size_t ind = 0; ind < indices.size(); ind += 3)
        {
            size_t vindex0 = indices[ind];
            size_t vindex1 = indices[ind + 1];
            size_t vindex2 = indices[ind + 2];

            float *base0 = vertexData + vindex0 * stride + positionOffset;
            float *base1 = vertexData + vindex1 * stride + positionOffset;
            float *base2 = vertexData + vindex2 * stride + positionOffset;

            polysTouchVertex[vindex0]++;
            polysTouchVertex[vindex1]++;
            polysTouchVertex[vindex2]++;

            glm::vec3 v0p(base0[0], base0[1], base0[2]);
            glm::vec3 v1p(base1[0], base1[1], base1[2]);
            glm::vec3 v2p(base2[0], base2[1], base2[2]);

            glm::vec3 u = v1p - v0p;
            glm::vec3 v = v2p - v0p;

            // FIXME:
            // u x v depends on winding order
            glm::vec3 normal = glm::cross(u, v);
            // uv?

            float *normalBase0 = vertexData + vindex0 * stride + normalOffset;
            float *normalBase1 = vertexData + vindex1 * stride + normalOffset;
            float *normalBase2 = vertexData + vindex2 * stride + normalOffset;

            normalBase0[0] += normal.x;
            normalBase0[1] += normal.y;
            normalBase0[2] += normal.z;

            normalBase1[0] += normal.x;
            normalBase1[1] += normal.y;
            normalBase1[2] += normal.z;

            normalBase2[0] += normal.x;
            normalBase2[1] += normal.y;
            normalBase2[2] += normal.z;
        }

        for (size_t vertex = 0; vertex < m_vb->getVerticesCount(); vertex++)
        {
            if (polysTouchVertex[vertex] >= 1)
            {
                float *normalBase = vertexData + vertex * stride + normalOffset;
                glm::vec3 n(normalBase[0], normalBase[1], normalBase[2]);

                n /= polysTouchVertex[vertex];

                n = utils::math::safeNormalize(n);

                memcpy(normalBase, glm::value_ptr(n), sizeof(float) * 3);
            }
        }
    }
    else
    {
        base::glog << "Cannot compute normals for triangle list mesh type." << base::logwarn;
    }
}

void SubMesh::computeTangentBasis()
{
    if (!m_vb)
        return;

    const auto &format = m_vb->getFormat();
    if (!format.hasComponent(VertexComponent::TANGENT) || 
        !format.hasComponent(VertexComponent::BITANGENT) || 
        !format.hasComponent(VertexComponent::POSITION) ||
        !format.hasComponent(VertexComponent::TEXTURE_COORDS) ||
        !format.hasComponent(VertexComponent::NORMAL))
        return;

    // Indexed.
    if (m_ib)
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
        float *vertexData = m_vb->getVertexData();
        size_t stride = m_vb->getFormat().getVertexSize() / sizeof(float);
        size_t tangentOffset = m_vb->getFormat().getOffsetTo(VertexComponent::TANGENT) / sizeof(float);
        size_t bitangentOffset = m_vb->getFormat().getOffsetTo(VertexComponent::BITANGENT) / sizeof(float);
        size_t posOffset = m_vb->getFormat().getOffsetTo(VertexComponent::POSITION) / sizeof(float);
        size_t txOffset = m_vb->getFormat().getOffsetTo(VertexComponent::TEXTURE_COORDS) / sizeof(float);
        size_t normalOffset = m_vb->getFormat().getOffsetTo(VertexComponent::NORMAL) / sizeof(float);

        for (size_t i = 0; i < m_vb->getVerticesCount(); i += 3)
        {
            float *base0 = vertexData + (i + 0) * stride;
            float *base1 = vertexData + (i + 1) * stride;
            float *base2 = vertexData + (i + 2) * stride;

            glm::vec3 v0p, v1p, v2p;
            memcpy(glm::value_ptr(v0p), base0 + posOffset, sizeof(float) * 3);
            memcpy(glm::value_ptr(v1p), base1 + posOffset, sizeof(float) * 3);
            memcpy(glm::value_ptr(v2p), base2 + posOffset, sizeof(float) * 3);

            glm::vec2 v0t, v1t, v2t;
            memcpy(glm::value_ptr(v0t), base0 + txOffset, sizeof(float) * 2);
            memcpy(glm::value_ptr(v1t), base1 + txOffset, sizeof(float) * 2);
            memcpy(glm::value_ptr(v2t), base2 + txOffset, sizeof(float) * 2);

            auto e1 = v1p - v0p;
            auto e2 = v2p - v0p;
            auto e1uv = v1t - v0t;
            auto e2uv = v2t - v0t;

            float r = 1.0f / (e1uv.x * e2uv.y - e1uv.y * e2uv.x);
            glm::vec3 tangent = (e1 * e2uv.y - e2 * e1uv.y) * r;
            glm::vec3 bitangent = (e2 * e1uv.x - e1 * e2uv.x) * r;

            for (size_t j = 0; j < 3; j++)
            {
                float *pt = vertexData + (i + j) * stride;

                (pt + tangentOffset)[0] = tangent.x;
                (pt + tangentOffset)[1] = tangent.y;
                (pt + tangentOffset)[2] = tangent.z;

                (pt + bitangentOffset)[0] = bitangent.x;
                (pt + bitangentOffset)[1] = bitangent.y;
                (pt + bitangentOffset)[2] = bitangent.z;
            }
        }

        for (size_t i = 0; i < m_vb->getVerticesCount(); i++)
        {
            // Gram-Schmidt orthogonalization.
            float *tangentPt = vertexData + i * stride + tangentOffset;
            float *bitangentPt = vertexData + i * stride + bitangentOffset;
            float *normalPt = vertexData + i * stride + normalOffset;

            glm::vec3 tangent(tangentPt[0], tangentPt[1], tangentPt[2]);
            glm::vec3 bitangent(bitangentPt[0], bitangentPt[1], bitangentPt[2]);
            glm::vec3 normal(normalPt[0], normalPt[1], normalPt[2]);

            tangent = tangent - normal * glm::dot(normal, tangent);
            tangent = utils::math::safeNormalize(tangent);

            if (glm::dot(glm::cross(normal, tangent), bitangent) < 0.0f)
            {
                tangent = tangent * -1.0f;
            }

            tangentPt[0] = tangent.x;
            tangentPt[1] = tangent.y;
            tangentPt[2] = tangent.z;
        }
    }
}

void SubMesh::setMaterial(shared_ptr<Material> material)
{
    if (!material)
    {
        base::glog << "Can not set empty material to a submesh" << base::logwarn;
        return;
    }

    m_material = material;
}

void SubMesh::setVertexBuffer(shared_ptr<VertexBuffer> vb)
{
    if (!vb)
    {
        base::glog << "Can not set empty vertex buffer to a submesh" << base::logwarn;
        return;
    }

    m_vb = vb;
}

void SubMesh::setIndexBuffer(shared_ptr<IndexBuffer> ib)
{
    if (!ib)
    {
        base::glog << "Can not set empty index buffer to a submesh" << base::logwarn;
        return;
    }

    m_ib = ib;
}

size_t SubMesh::getTrianglesCount() const
{
    if (m_ib)
    {
        // Indexed.
        return m_ib->getIndices().size() / 3;
    }
    else
    {
        return m_vb->getElementsUsed() / 3;
    }
}

} }
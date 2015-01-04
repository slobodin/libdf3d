#pragma once

namespace df3d { namespace render {

class Material;
class VertexBuffer;
class IndexBuffer;

class SubMesh : boost::noncopyable
{
    shared_ptr<Material> m_material;
    shared_ptr<VertexBuffer> m_vb;
    shared_ptr<IndexBuffer> m_ib;

public:
    SubMesh();

    //! Computes normals for vertices.
    void computeNormals();
    //! Compute tangent space basis vectors.
    void computeTangentBasis();

    void setMaterial(shared_ptr<Material> material);
    void setVertexBuffer(shared_ptr<VertexBuffer> vb);
    void setIndexBuffer(shared_ptr<IndexBuffer> ib);

    size_t getTrianglesCount() const;

    shared_ptr<Material> getMaterial() { return m_material; }
    shared_ptr<VertexBuffer> getVertexBuffer() { return m_vb; }
    shared_ptr<IndexBuffer> getIndexBuffer() { return m_ib; }
};

} }
#pragma once

FWD_MODULE_CLASS(render, IndexBuffer)
FWD_MODULE_CLASS(render, VertexBuffer)

namespace df3d { namespace utils {

class MeshIndexer
{
public:
    MeshIndexer();
    ~MeshIndexer();

    //! Indexes vertex buffer, returns resulted indexes.
    shared_ptr<render::IndexBuffer> index(shared_ptr<render::VertexBuffer> vb);
};

} }
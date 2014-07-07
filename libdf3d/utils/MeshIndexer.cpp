#include "df3d_pch.h"
#include "MeshIndexer.h"

#include <render/Material.h>
#include <render/VertexIndexBuffer.h>

namespace df3d { namespace utils {

MeshIndexer::MeshIndexer()
{
}

MeshIndexer::~MeshIndexer()
{
}

shared_ptr<render::IndexBuffer> MeshIndexer::index(shared_ptr<render::VertexBuffer> vb)
{
    // TODO:
    assert(false);
    return nullptr;

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

} }
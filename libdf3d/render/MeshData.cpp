#include "df3d_pch.h"
#include "MeshData.h"

#include "SubMesh.h"
#include <base/Controller.h>
#include <resources/ResourceManager.h>

namespace df3d { namespace render {

MeshData::MeshData()
{
}

MeshData::~MeshData()
{
}

void MeshData::attachSubMesh(shared_ptr<SubMesh> submesh)
{
    if (!submesh)
    {
        base::glog << "Can not attach empty submesh to a mesh data" << base::logdebug;
        return;
    }

    m_submeshes.push_back(submesh);

    m_trianglesCount += submesh->getTrianglesCount();
}

void MeshData::clear()
{
    if (!valid())
    {
        base::glog << "Can not clear invalid mesh data" << base::logwarn;
        return;
    }

    m_submeshes.clear();
    m_trianglesCount = 0;
}

void MeshData::setMaterial(shared_ptr<render::Material> newMaterial)
{
    if (!valid())
    {
        base::glog << "Can't set material because mesh is not valid" << base::logwarn;
        return;
    }

    if (!newMaterial)
    {
        base::glog << "Trying set empty material to the mesh" << base::logwarn;
        return;
    }

    for (auto s : m_submeshes)
        s->setMaterial(newMaterial);
}

void MeshData::computeNormals()
{
    for (auto sm : m_submeshes)
        sm->computeNormals();
}

bool MeshData::init()
{
    // TODO:
    // Compute normals?
    
    //assert(false);

    return true;
}

shared_ptr<MeshData> MeshData::clone() const
{
    if (!valid())
    {
        base::glog << "Can not clone invalid meshdata" << base::logwarn;
        return nullptr;
    }

    auto result = make_shared<render::MeshData>();
    result->setInitialized();

    for (auto sm : m_submeshes)
    {
        auto newSubMesh = make_shared<render::SubMesh>();

        // Share vertex and index buffer.
        if (sm->getVertexBuffer())
            newSubMesh->setVertexBuffer(sm->getVertexBuffer());
        if (sm->getIndexBuffer())
            newSubMesh->setIndexBuffer(sm->getIndexBuffer());
        if (sm->getMaterial())
            newSubMesh->setMaterial(sm->getMaterial());

        result->attachSubMesh(newSubMesh);
    }

    g_resourceManager->appendResource(result);

    return result;
}

} }
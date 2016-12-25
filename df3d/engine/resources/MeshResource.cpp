#include "MeshResource.h"

#include "loaders/MeshLoader_obj.h"
#include "loaders/MeshLoader_dfmesh.h"
#include "ResourceFileSystem.h"
#include "ResourceDataSource.h"
#include <df3d/engine/EngineController.h>
#include <df3d/engine/io/FileSystemHelpers.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFileSystem.h>
#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/lib/JsonUtils.h>

namespace df3d {

static MeshResourceData *LoadMeshDataFromFile(const char *path, Allocator &allocator)
{
    auto meshDataSource = svc().resourceManager().getFS().open(path);
    if (!meshDataSource)
        return nullptr;

    MeshResourceData *result = nullptr;
    if (FileSystemHelpers::compareExtension(path, ".obj"))
        result = MeshLoader_obj(*meshDataSource, allocator);
    else if (FileSystemHelpers::compareExtension(path, ".dfmesh"))
        result = MeshLoader_dfmesh(*meshDataSource, allocator);
    else
        DF3D_ASSERT_MESS(false, "Unsupported mesh file format!");

    svc().resourceManager().getFS().close(meshDataSource);

    return result;
}

static void BoundingVolumeFromGeometry(BoundingVolume *volume, const MeshResourceData &resource)
{
    volume->reset();

    // Compute volume.
    for (const auto &meshpart : resource.parts)
    {
        const auto &vertexData = meshpart->vertexData;

        for (size_t i = 0; i < vertexData.getVerticesCount(); i++)
        {
            auto &vdata = const_cast<VertexData&>(vertexData);
            auto v = (glm::vec3*)vdata.getVertexAttribute(i, VertexFormat::POSITION);

            if (v)
                volume->updateBounds(*v);
        }
    }
}

void MeshHolder::listDependencies(ResourceDataSource &dataSource, std::vector<std::string> &outDeps)
{
    Json::Value root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return;

    if (root.isMember("material_lib"))
        outDeps.push_back(root["material_lib"].asString());
}

bool MeshHolder::decodeStartup(ResourceDataSource &dataSource, Allocator &allocator)
{
    Json::Value root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return false;

    if (root.isMember("material_lib"))
        m_materialLib = Id(root["material_lib"].asCString());

    m_resourceData = LoadMeshDataFromFile(root["path"].asCString(), allocator);

    return m_resourceData != nullptr;
}

void MeshHolder::decodeCleanup(Allocator &allocator)
{
    DestroyMeshData(m_resourceData, allocator);
    m_resourceData = nullptr;
}

bool MeshHolder::createResource(Allocator &allocator)
{
    m_resource = MAKE_NEW(allocator, MeshResource)();
    m_resource->materialLibResourceId = m_materialLib;

    auto &backend = svc().renderManager().getBackend();

    for (auto part : m_resourceData->parts)
    {
        m_resource->materialNames.push_back(part->materialName);
        MeshPart hwPart;

        auto &vData = part->vertexData;
        hwPart.vertexBuffer = backend.createVertexBuffer(vData, GpuBufferUsageType::STATIC);

        if (part->indices.size() > 0)
        {
            hwPart.indexBuffer = backend.createIndexBuffer(part->indices.size(), part->indices.data(), GpuBufferUsageType::STATIC, INDICES_32_BIT);
            hwPart.numberOfElements = part->indices.size();
        }
        else
        {
            hwPart.numberOfElements = vData.getVerticesCount();
        }

        m_resource->meshParts.push_back(hwPart);
    }

    BoundingVolumeFromGeometry(&m_resource->localAABB, *m_resourceData);
    BoundingVolumeFromGeometry(&m_resource->localBoundingSphere, *m_resourceData);
    m_resource->convexHull.constructFromGeometry(*m_resourceData, allocator);

    return true;
}

void MeshHolder::destroyResource(Allocator &allocator)
{
    for (const auto &hwPart : m_resource->meshParts)
    {
        svc().renderManager().getBackend().destroyVertexBuffer(hwPart.vertexBuffer);
        if (hwPart.indexBuffer.isValid())
            svc().renderManager().getBackend().destroyIndexBuffer(hwPart.indexBuffer);
    }

    MAKE_DELETE(allocator, m_resource);
    m_resource = nullptr;
}

MeshResourceData *LoadMeshDataFromFile_Workaround(const char *path, Allocator &allocator)
{
    Json::Value root = JsonUtils::fromFile(path);
    if (root.isNull())
        return nullptr;

    return LoadMeshDataFromFile(root["path"].asCString(), allocator);
}

void DestroyMeshData(MeshResourceData *resource, Allocator &allocator)
{
    for (auto part : resource->parts)
        MAKE_DELETE(allocator, part);
    MAKE_DELETE(allocator, resource);
}

}

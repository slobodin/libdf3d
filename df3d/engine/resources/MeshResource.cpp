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

bool MeshHolder::decodeStartup(ResourceDataSource &dataSource, Allocator &allocator)
{
    Json::Value root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return false;

    if (root.isMember("material_lib"))
        m_materialLib = root["material_lib"].asString();

    auto path = root["path"].asString();

    const auto extension = FileSystemHelpers::getFileExtension(path);

    auto meshSource = svc().resourceManager().getFS().open(path.c_str());
    if (!meshSource)
        return false;

    if (extension == ".obj")
        m_resourceData = MeshLoader_obj(*meshSource, allocator);
    else if (extension == ".dfmesh")
        m_resourceData = MeshLoader_dfmesh(*meshSource, allocator);
    else
        DFLOG_WARN(false, "Unsupported mesh file format!");

    svc().resourceManager().getFS().close(meshSource);

    if (!m_materialLib.empty())
        svc().resourceManager().loadResource(m_materialLib);

    return m_resourceData != nullptr;
}

void MeshHolder::decodeCleanup(Allocator &allocator)
{
    for (auto part : m_resourceData->parts)
        allocator.makeDelete(part);
    allocator.makeDelete(m_resourceData);
    m_resourceData = nullptr;
}

bool MeshHolder::createResource(Allocator &allocator)
{
    m_resource = allocator.makeNew<MeshResource>();
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

    allocator.makeDelete(m_resource);
    m_resource = nullptr;
}

}

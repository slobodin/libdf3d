#pragma once

#include <df3d/engine/render/RenderCommon.h>
#include <df3d/engine/render/Vertex.h>
#include <df3d/lib/math/AABB.h>
#include <df3d/lib/math/BoundingSphere.h>
#include <df3d/lib/math/ConvexHull.h>
#include "IResourceHolder.h"

namespace df3d {

struct MeshResourceData
{
    struct Part
    {
        VertexData vertexData;
        PodArray<uint32_t> indices;
        IndicesType indicesType;
        std::string materialName;

        Part(const VertexFormat &vf, Allocator &alloc) : vertexData(vf), indices(alloc) { }
    };

    std::vector<Part*> parts;
};

struct MeshPart
{
    VertexBufferHandle vertexBuffer;
    IndexBufferHandle indexBuffer;
    size_t numberOfElements = 0;
};

struct MeshResource
{
    std::vector<MeshPart> meshParts;
    std::vector<Id> materialNames;
    Id materialLibResourceId;
    AABB localAABB;
    BoundingSphere localBoundingSphere;
    ConvexHull convexHull;
};

class MeshHolder : public IResourceHolder
{
    MeshResourceData *m_resourceData = nullptr;
    MeshResource *m_resource = nullptr;
    Id m_materialLib;

public:
    void listDependencies(ResourceDataSource &dataSource, std::vector<std::string> &outDeps);
    bool decodeStartup(ResourceDataSource &dataSource, Allocator &allocator) override;
    void decodeCleanup(Allocator &allocator) override;
    bool createResource(Allocator &allocator) override;
    void destroyResource(Allocator &allocator) override;

    void* getResource() override { return m_resource; }
};

MeshResourceData *LoadMeshDataFromFile_Workaround(const char *path, Allocator &allocator);
void DestroyMeshData(MeshResourceData *resource, Allocator &allocator);

}

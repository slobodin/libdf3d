#pragma once

namespace df3d {

struct MeshResourceData;
class ResourceDataSource;

MeshResourceData* MeshLoader_fbx(ResourceDataSource &dataSource, Allocator &alloc);

}

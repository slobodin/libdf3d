#pragma once

namespace df3d {

struct AnimatedMeshResourceData;
class ResourceDataSource;

AnimatedMeshResourceData* MeshLoader_assxml(ResourceDataSource &dataSource, Allocator &alloc);

}

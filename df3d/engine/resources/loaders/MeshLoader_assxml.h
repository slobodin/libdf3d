#pragma once

namespace df3d {

struct MeshResourceData;
class ResourceDataSource;

MeshResourceData* MeshLoader_assxml(ResourceDataSource &dataSource, Allocator &alloc);

}

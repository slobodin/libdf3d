#pragma once

namespace df3d { 

struct MeshResourceData;
class ResourceDataSource;

MeshResourceData* MeshLoader_obj(ResourceDataSource &dataSource, Allocator &alloc);

}

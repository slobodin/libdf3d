#pragma once

namespace df3d { 

struct MeshResourceData;
class ResourceDataSource;
class Allocator;

MeshResourceData* MeshLoader_obj(ResourceDataSource &dataSource, Allocator &alloc);

}

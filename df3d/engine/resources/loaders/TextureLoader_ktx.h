#pragma once

namespace df3d {

class ResourceDataSource;
struct TextureResourceData;

TextureResourceData* TextureLoader_ktx(ResourceDataSource &dataSource, Allocator &alloc);

}

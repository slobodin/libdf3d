#pragma once

namespace df3d {

class ResourceDataSource;
struct TextureResourceData;

TextureResourceData* TextureLoader_pvrtc(ResourceDataSource &dataSource, Allocator &alloc);

}

#pragma once

namespace df3d {

class ResourceDataSource;
struct TextureResourceData;
class Allocator;

TextureResourceData* TextureLoader_pvrtc(ResourceDataSource &dataSource, Allocator &alloc);

}

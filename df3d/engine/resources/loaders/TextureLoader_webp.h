#pragma once

namespace df3d {

struct TextureResourceData;
class Allocator;
class ResourceDataSource;

TextureResourceData* TextureLoader_webp(ResourceDataSource &dataSource, Allocator &alloc, bool forceRGBA);

}
